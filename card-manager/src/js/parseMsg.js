function parseMsg(topic, msg) {\
    print ('parseMsg running');\
    switch (topic) {\
        case '/card':\
            switch (msg.cmd) {\
                case 'new_card':\
                    newCard(msg);\
                    break;\
                case 'update_card':\
                    updateCard(msg);\
                    break;\
                case 'select_card':\
                    print ('select_card command received');\
                    if (msg.action != null) {\
                        selectCard(msg.action);\
                    }\
                    break;\
                default:\
                    print ('no cmd specified!');\
                    break;\
            }\
            break;\
        case '/status':\
            switch (msg.cmd) {\
                case 'update':\
                    print ('update command received');\
                    if (msg.elements && Array.isArray(msg.elements)) {\
                        for (var i = 0; i < msg.elements.length; i++) {\
                            if (msg.elements[i].type != null && msg.elements[i].value != null) {\
                                print('element ' + i + ': ' + msg.elements[i].type + ' ' + msg.elements[i].value);\
                                if (typeof msg.elements[i].value === 'number') {\
                                    print('value is a number!');\
                                    setStatus(msg.elements[i].type, msg.elements[i].value);\
                                }\
                            }\
                        }\
                    }\
                    break;\
                default:\
                    print ('no cmd specified!');\
                    break;\
            }\
            break;\
        default:\
            print ('topic not recognized specified!');\
            break;\
    }\
}