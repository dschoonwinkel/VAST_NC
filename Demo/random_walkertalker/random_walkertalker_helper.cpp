//random_walkertalker_helper.cpp

#include <iostream>
#include <string>

#include "random_walkertalker.h"
using namespace Vast;

void send_to_neighbours (VAST* vastclient, string to_send) {
// send off input to AOI neighbors
    // note: self is also included as the 1st neighbor
    Message send_msg (12);

    send_msg.clear (123);
//    send_msg.store(vastclient->getSelf()->id);
//    send_msg.from = vastclient->getSelf()->id;    // This is done automatically by the VASTClient
    send_msg.store (to_send.c_str(), to_send.length(), true);

    /* SEND-based */
    vector<Node *>&nodes = vastclient->list ();

    for (size_t j=0; j < nodes.size (); j++)
    {
//        Position pos = nodes[j]->aoi.center;
        Vast::id_t id      = nodes[j]->id;
        send_msg.addTarget (id);
    }
    printMessage(send_msg);
    // send away message
    vastclient->send (send_msg);


}

void gen_random_str(char *s, const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

const char *nouns[] = {"area","book","business","case","child","company","country","day","eye","fact","family","government","group","hand","home","job","life","lot","man","money","month","mother","Mr","night","number","part","people","place","point","problem","program","question","right","room","school","state","story","student","study","system","thing","time","water","way","week","woman","word","work","world","year"};
const char *verbs[] = {"ask","be","become","begin","call","can","come","could","do","feel","find","get","give","go","have","hear","help","keep","know","leave","let","like","live","look","make","may","mean","might","move","need","play","put","run","say","see","seem","should","show","start","take","talk","tell","think","try","turn","use","want","will","work","would"};
const char *articles[] = {"the", "a", "one", "some", "any"};
const char *prepositions[] = {"aboard","about","above","across","after","against","along","amid","among","anti","around","as","at","before","behind","below","beneath","beside","besides","between","beyond","but","by","concerning","considering","despite","down","during","except","excepting","excluding","following","for","from","in","inside","into","like","minus","near","of","off","on","onto","opposite","outside","over","past","per","plus","regarding","round","save","since","than","through","to","toward","towards","under","underneath","unlike","until","up","upon","versus","via","with","within","without"};

void gen_random_sentence(char *sentence, const size_t len) {

    static const size_t n_articles = sizeof articles /sizeof articles[0];
    static const size_t n_nouns = sizeof nouns /sizeof nouns[0];
    static const size_t n_verbs = sizeof verbs /sizeof verbs[0];
    static const size_t n_prepositions = sizeof prepositions /sizeof prepositions[0];

    static const char **words[] = {articles, nouns, verbs, prepositions};
    static const size_t word_lenghts[] = {n_articles, n_nouns, n_verbs, n_prepositions};
    static const size_t wordtypes_num = sizeof words /sizeof words[0];

    for (size_t i = 0, word_type = 0 ; i < len; word_type++) {
        word_type = word_type % wordtypes_num;
        int num = rand() % word_lenghts[word_type];
        debug_print("To add: %s\n", words[word_type][num]);
        debug_print("Len add: %lu\n", strlen(words[word_type][num]));
        if (strlen(words[word_type][num])+1 + i < len) {
            strcpy(sentence+i, words[word_type][num]);
            i += strlen(words[word_type][num]);
            debug_print("i after word add : %lu\n", i);
            sentence[i] = '\0';
        }
        else {
            //If you can't fit another word, stop adding words.
            break;
        }
        debug_print("Str result: %s\n", sentence);

        if (i < len - 1) {
            strcat(sentence, " ");
            i++;
        }

        debug_print("After space add: i: %lu\n", i);

    }
}

void printMessage(Vast::Message msg) {
    std::cout << "Message details" << std::endl;
    std::cout << "from: " << msg.from << std::endl;
    std::cout << "size: " << msg.size << std::endl;
    std::cout << "msgtype: " << msg.msgtype << std::endl;
    std::cout << "msggroup: " << int(msg.msggroup) << std::endl;
    std::cout << "priority: " << int(msg.priority) << std::endl;
    std::cout << "reliable: " << int(msg.reliable) << std::endl;
    std::cout << "Targets: " << std::endl;
    for (Vast::id_t target : msg.targets)
    {
        std::cout << target << std::endl;
    }
    for (size_t i = 0; i < msg.size; i++)
    {
        char a = msg.data[i];
        if (a >= ' ' && a <= 'z') {
            std::cout << a;
        }
        else {
            std::cout << int(a);
        }
        std::cout << " ";
    }
    std::cout << std::endl;
//    std::cout << "data: " << string(msg.data) << std::endl;
}


//   fa 16 00 50 -- Header bytes, four parts: start_seq = 1010, msg_type = 3 Regular, msg_size = 0x187, end_seq = 0101
//   01 00 0e 04 01 01 00 7f -- from id_t = 9151315546691469313
//   41 00 00 00 -- size_t = 65 (probably with all the extra bits
//   2f 7b -- msg_type = SEND type (i.e. for particular targets) + 123 - demo message type
//   02 -- msg_group 2 = MATCHER MESSAGE
//   03 -- Priority 3
//   01 -- Reliable, yes
//   01 -- Num of targets
//   01 00 0d 04 01 01 00 7f -- Target - Matcher - yes, this is the matcher ID
//   2c 00 00 00 -- store size variable, 4 bytes = 44 -> there are 44 characters
//
//   6f 6e 65 20 63 68 69 6c
//   64 20 68 65 6c 70 20 69 
//   6e 74 6f 20 61 6e 79 20 
//   74 69 6d 65 20 68 65 6c 
//   70 20 61 62 6f 76 65 20 
//   74 68 65 20 -- Text message = one child help into any time help above the
//
//
//   02 40 0d 04 01 01 00 7f     -- One of the nodes
//   02 40 0e 04 01 01 00 7f     -- Other node
//   02                          -- Number of stored IDs, used by Matcher


