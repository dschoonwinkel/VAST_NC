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

#ifndef VAST_NETWORK_HANDLER_H
#define VAST_NETWORK_HANDLER_H

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#ifdef WIN32
// disable warning about C4996: 'sprintf' was declared deprecated
#pragma warning(disable: 4996)
#endif

#include "ace/INET_Addr.h"
#include "ace/Event_Handler.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Dgram.h"         // ACE_INET_Dgram
#include "ace/Reactor.h"

#include "VASTTypes.h"
#include "VASTBuffer.h"
#include "net_manager.h"

#ifdef VAST_USE_SSL
#include "ace/SSL/SSL_SOCK_Stream.h"    // ACE_SSL_SOCK_Stream
#endif

namespace Vast {

class net_ace_handler : public ACE_Event_Handler
{

public:
    net_ace_handler (); 
    
    int open (ACE_Reactor *reactor, void *msghandler, id_t remote_id = 0);

    // close connection & unregister from reactor
    int close (void);

#ifdef VAST_USE_SSL
    // a trick to let acceptor thinks this is a ACE_SOCK_Stream object
    ACE_SSL_SOCK_Stream &getSSLStream ()
    {
        _secure = true;
        return this->_ssl_stream;
    }
#endif

    // a trick to let acceptor thinks this is a ACE_SOCK_Stream object
    operator ACE_SOCK_Stream & ()
    {
        return this->_stream;
    }

    // open a UDP sender / receiver at a given port
    ACE_SOCK_Dgram *openUDP (ACE_INET_Addr addr);

    // obtain address of remote host
    IPaddr &getRemoteAddress ();

    // swtich remote ID to a new one
    bool switchRemoteID (id_t oldID, id_t newID);
        
protected:

    // standard method to provide connection handle to reactor
    ACE_HANDLE get_handle (void) const 
    {
        if (_udp == NULL)
        {
#ifdef VAST_USE_SSL
            if (_secure)
                return this->_ssl_stream.get_handle ();
            else
#endif
                return this->_stream.get_handle ();
        }
        else
            return this->_udp->get_handle ();
    }

    // handling incoming message
    virtual int handle_input (ACE_HANDLE fd);

    // if handle_input() returns -1, reactor would call handle_close()
    int handle_close (ACE_HANDLE, ACE_Reactor_Mask mask);

private:

    // this will force the class to be allocated dynamically (p.71 in ACE Tutorial)
    ~net_ace_handler () 
    {
        // remove UDP listener
        if (_udp != NULL)
            delete _udp;
    }

    // info the remote node
    id_t            _remote_id;
    IPaddr          _remote_addr;

    // objects for send/recv TCP/UDP streams
    ACE_SOCK_Stream _stream;

    // secure connection stream
    bool                _secure;
#ifdef VAST_USE_SSL
    ACE_SSL_SOCK_Stream _ssl_stream;
#endif
    
    ACE_SOCK_Dgram  *_udp;
    ACE_INET_Addr   _local_addr;

    // the same reactor as the acceptor's
    ACE_Reactor     *_reactor;
    void            *_msghandler;

    // generic buffer
    VASTBuffer       _buf;
};

} // end namespace Vast

#endif // VAST_NETWORK_HANDLER_H

