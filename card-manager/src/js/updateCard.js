function updateCard(msg) { \
    print ('update_card command received');\
    if (msg.id !== null && msg.elements && Array.isArray(msg.elements)) {\
        print('card ' + msg.id + ': elements array has length of ' + msg.elements.length);\
        for (var i = 0; i < msg.elements.length; i++) {\
            print('element ' + i + ' being updated to ' + msg.elements[i].value);\
            if (msg.elements[i].id !== null && msg.elements[i].value !== null && msg.elements[i].type !== 'calendar') {\
                updateElement(  msg.id,\
                                msg.elements[i].id,\
                                msg.elements[i].value\
                            );\
            }else{\
                updateCalendar( msg.id,\
                                msg.elements[i].id,\
                                msg.elements[i].date.year || 1970,\
                                msg.elements[i].date.month || 1,\
                                msg.elements[i].date.day || 1\
                            );\
            }\
        }\
    }\
}
