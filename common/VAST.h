/*
 * VAST, a scalable peer-to-peer network for virtual environments
 * Copyright (C) 2005-2011 Shun-Yun Hu (syhu@ieee.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/*
 *  VAST.h -- main VAST header, used by application     
 *
 *  history 2005/04/11  ver 0.1
 *          2007/01/11  ver 0.2     simplify interface   
 *			2008/08/20  ver 0.3		re-written for generic overlay
 *          2009/04/02  ver 0.4		re-defined for SPS
 *          2010/02/09  ver 0.5     re-structured to separate "matcher" from "relay"
 */

#ifndef _VAST_H
#define _VAST_H

#include "Config.h"
#include "VASTTypes.h"
#include "Voronoi.h"
#include "MessageHandler.h"

using namespace std;

namespace Vast
{
    // NOTE: we start the message number slightly higher than VONPeer
    typedef enum 
    {
        // internal message # must begin with VON_MAX_MSG as VONpeer is used and share the MessageQueue        
        MATCHER_CANDIDATE = VON_MAX_MSG,// notify gateway of willingness to be origin matcher
        MATCHER_INIT,                   // call up a candidate origin matcher to start up a new world
        MATCHER_ALIVE,                  // keepalive messages from matchers to gateway
        MATCHER_WORLD_INFO,               // notify a matcher of its world_id
        NOTIFY_MATCHER,                 // current matcher notifying client of new current matcher
        NOTIFY_CLOSEST,                 // current matcher notifying client of closest alternative matcher
        JOIN,                           // client request to gateway for joining a world (find first "origin matcher")
        LEAVE,                          // departure of a client
        PUBLISH,                        // publish a message         
        SUBSCRIBE,                      // send subscription
        SUBSCRIBE_R,                    // to reply whether a node has successfully subscribed (VON node joined)        
        SUBSCRIBE_TRANSFER,             // transfer a subscription to a neighbor matcher
        SUBSCRIBE_UPDATE,               // update of a subscription to neighboring matchers
        MOVE,                           // position update to normal nodes
        MOVE_F,                         // full update for an AOI region        
        NEIGHBOR,                       // send back a list of known neighbors
        NEIGHBOR_REQUEST,               // request full info for an unknown neighbor
        SEND,                           // send a particular message to certain targets        
        ORIGIN_MESSAGE,                 // messsage to origin matcher
        MESSAGE,                        // deliver a message to a node
        SUBSCRIBE_NOTIFY,               // client notifying a relay of its subscription
        STAT,                           // sending statistics for gateway to record
        SYNC_CLOCK,                     // synchronize logical clock with gateway

        // Relay-specific messages
        REQUEST,                // request for relays
        RELAY,                  // notifying an existing relay        
        PING,                   // query to measure latency
        PONG,                   // reponse to PING
        PONG_2,                 // reponse to PONG
        RELAY_QUERY,            // find closest relay
        RELAY_QUERY_R,          // response to closest relay query
        RELAY_JOIN,             // attach to the physically closest relay
        RELAY_JOIN_R,           // response to JOIN request

    } VAST_Message;

    // default world ID (lobby) for VAST
    #define VAST_DEFAULT_WORLD_ID   (1)

    //
    // NOTE that we currently assume two things are decided
    //      outside of the VAST class:
    //
    //          1. unique ID (hostID obtained via VASTnet)
    //          2. a coordinate representing physical location (Vivaldi)
    //      
    //      Also, a VAST node (the same one) serves as both a Client at a regular node
    //      and also a Relay/Matcher at super-peers
    //
    class  VAST : public MessageHandler
    {

    public:

		//
		// main VAST methods
		//
        // NOTE: there are two timeout values that need attention when debugging,
        //       as they would disconnect a client and cause re-subscription.
        //
        //          TIMEOUT_REMOVE_GHOST        in VASTClient.h
        //          TIMEOUT_REMOVE_CONNECTION   in VASTnet.h

        // Warning: passing STL across DLL boundary may create problems, see:
        // http://www.gamedev.net/community/forums/topic.asp?topic_id=486103

        VAST ()
            :MessageHandler (MSG_GROUP_VAST_CLIENT)
        {
        }
         
        //virtual ~VAST () = 0;
        
        // join by specifying the gateway to contact, and an optional worldID (default to be 1)
        virtual bool            join (const IPaddr &gateway, world_t worldID) = 0;

        // quit the overlay
        virtual void            leave () = 0;
        
        // specify a subscription area for point or area publications 
        virtual bool            subscribe (Area &area, layer_t layer) = 0;

        // send a message to all subscribers within a publication area
        virtual bool            publish (Area &area, layer_t layer, Message &message) = 0;
            
        // move a subscription area to a new position
        // returns actual AOI in case the position is already taken, or NULL if subscription does not exist
        virtual Area *          move (id_t subID, Area &aoi, bool update_only = false) = 0;

        // send a custom message to a particular VAST node (ID obtained from list ())
        // returns the number of successful send, 
        // optionally obtains the failed targets, and indicate if the send should be 'directly' (end-to-end, without relays)
        // TODO: send () now only sends to matcher, so target is always 1
        virtual int             send (Message &message, vector<id_t> *failed = NULL, bool direct = false) = 0;

        // obtain a list of subscribers with an area
        virtual vector<Node *>& list (Area *area = NULL) = 0;

        // obtain a list of physically closest hosts
        virtual vector<Node *>& getPhysicalNeighbors () = 0;

        // obtain a list of logically closest hosts (a subset of nodes by list () that are relay-level)
        virtual vector<Node *>& getLogicalNeighbors () = 0;

        // get a message from the network queue
        virtual Message *       receive () = 0;

        // report some message to gateway (to be processed or recorded)
        virtual bool            reportGateway (Message &message) = 0;
    
        // report some message to origin matcher
        virtual bool            reportOrigin (Message &message) = 0;

        // get current statistics about this node (a NULL-terminated string)
        virtual char *          getStat (bool clear = false) = 0;
        
        // get the current node's information
        virtual Node *          getSelf () = 0;

        // subscription id indicates subscribe success
        virtual id_t            getSubscriptionID () = 0;

        // get the world_id I'm currently joining
        virtual world_t         getWorldID () = 0;

        // whether the current node is joined (part of relay mesh)
        virtual bool            isJoined () = 0;

        // whether I am a relay node
        virtual int             isRelay () = 0;

        // whether I have public IP
        virtual bool            hasPublicIP () = 0;
        
        //
        // accessor (non-essential) functions for GUI display
        //

        // obtain access to Voronoi class (usually for drawing purpose)
        // returns NULL if matcher does not exist on this node
        //virtual Voronoi *getVoronoi () = 0;

        // 
        // stat collectors
        //
        // TODO: remove stat from VAST interface?
        
        // get message latencies, currently support PUBLISH & MOVE
        // msgtype == 0 indicates clear up existing latency records
        virtual StatType *getMessageLatency (msgtype_t msgtype) = 0;

        static std::string MSGTYPEtoString(msgtype_t msgtype);
    };



} // end namespace Vast



#endif // VAST_H
