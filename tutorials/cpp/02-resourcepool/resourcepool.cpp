#include <cstdio>
#include <iostream>
#include <mama/mamacpp.h>
#include <mama/MamaResourcePool.h>
#include <memory>

using namespace Wombat;

// Rudimentary error checking mechanism for brevity
void checkStatusAndExitOnFailure(mama_status status, const char* scenario) {
    if (status != MAMA_STATUS_OK) {
        std::cerr << "Failure encountered during "
                  << scenario
                  << ": "
                  << MamaStatus(status).toString()
                  << std::endl;
        exit(EXIT_FAILURE);
    }
}

void usageAndExit(const char* appName) {
    fprintf(stderr,
            "This is an OpenMAMA market data subscriber which assumes an\n"
            "OpenMAMA compatible data source is available to connect to and is\n"
            "configured in mama.properties.\n\n"
            "Usage: %s -u [uri]\n\n"
            "Arguments:\n"
            "\t-d [dictionary]\tDictionary file to load. Default: [/opt/openmama/data/dictionaries/data.dict]\n"
            "\t-S [source]\tSource name (prefix) to use. Default: [OM]\n"
            "\t-t [transport]\tTransport from mama.properties to use. Default: [sub]\n"
            "\t-u [uri]\tURI representing subscription in `bridge://transport/source/topic` format\n"
            "\t-v\t\tEnable verbose logging\n"
            "\t-B\t\tDisables dictionary request\n"
            "\t-I\t\tPrevents an initial from being requested\n"
            "\n",
            appName);
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
    // OpenMAMA status variable which we will use throughout this example
    const char* transportName = "sub";
    const char* sourceName = "OM";
    const char* symbolName = NULL;
    const char* uri = NULL;
    const char* dictionaryFile = "/opt/openmama/data/dictionaries/data.dict";
    int requiresDictionary = 1;
    int requiresInitial = 1;

    // Parse out command line options
    int i = 0;
    for (i = 1; i < argc; )
    {
        if (strcmp ("-B", argv[i]) == 0)
        {
            requiresDictionary = 0;
        }
        else if (strcmp ("-d", argv[i]) == 0)
        {
            dictionaryFile = argv[++i];
        }
        else if (strcmp ("-I", argv[i]) == 0)
        {
            requiresInitial = 0;
        }
        else if (strcmp ("-s", argv[i]) == 0)
        {
            symbolName = argv[++i];
        }
        else if (strcmp ("-S", argv[i]) == 0)
        {
            sourceName = argv[++i];
        }
        else if (strcmp ("-t", argv[i]) == 0)
        {
            transportName = argv[++i];
        }
        else if (strcmp ("-u", argv[i]) == 0)
        {
            uri = argv[++i];
        }
        else if (strcmp ("-v", argv[i]) == 0)
        {
            // Set up the Log level for OpenMAMA internals
            mama_setLogLevel(MAMA_LOG_LEVEL_FINEST);
        }
        else if ((strcmp (argv[i], "-h") == 0)||
                 (strcmp (argv[i], "--help") == 0)||
                 (strcmp (argv[i], "-?") == 0))
        {
            usageAndExit(argv[0]);
        }
        else
        {
            fprintf(stderr, "Unknown arg: %s\n",    argv[i]);
            usageAndExit(argv[0]);
        }
        ++i;
    }

    try {
        std::unique_ptr<MamaResourcePool> pool (new MamaResourcePool("default"));

        // Set up the data dictionary
        MamaDictionary* dictionary = nullptr;
        if (requiresDictionary) {
            dictionary = new MamaDictionary();
            dictionary->populateFromFile(dictionaryFile);
        }

        class SubscriptionEventHandler : public MamaSubscriptionCallback
        {
            void onMsg (MamaSubscription* subscription, MamaMsg& msg) override {
                auto dictionary = (MamaDictionary*)subscription->getClosure();
                std::cout << msg.toJsonString (dictionary) << std::endl;
//                static int msgsReceived = 0;
//                if (++msgsReceived == 10) {
//                    Mama::stopAll();
//                }
            }
        };

        // Initialize instance of the event handler
        SubscriptionEventHandler eventHandler;

        // The OpenMAMA subscription reference to be populated
        MamaSubscription* subscription;
        if (nullptr != uri) {
            subscription = pool->createSubscriptionFromUri (
                uri,
                &eventHandler,
                dictionary);
        } else {
            subscription = pool->createSubscriptionFromComponents (
                transportName,
                sourceName,
                symbolName,
                &eventHandler,
                dictionary);
        }

        if (requiresInitial) {
            subscription->setRequiresInitial (requiresInitial);
        }

        subscription->activate();

        // Kick off OpenMAMA now (blocking call, non-blocking call also available)
        Mama::startAll();

        delete dictionary;
        // ...pool will delete when it leaves scope after start is unblocked
    } catch (MamaStatus const & status) {
        std::cerr << "Exception MamaStatus: " << status.toString () << std::endl;
    } catch (...) {
        std::cerr << "Unexpected exception found!" << std::endl;
    }
}
