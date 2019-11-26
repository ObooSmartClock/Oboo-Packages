function newCard(msg) { \
    print ('new_card command received');\
    if (msg.bg_color != null && msg.elements && Array.isArray(msg.elements) && msg.responseTopic) {\
        print('lets make a new card');\
        var cardId = createCard(msg.bg_color);\
        print('elements array has length of ' + msg.elements.length);\
        for (var i = 0; i < msg.elements.length; i++) {\
            print('element ' + i + ': ' + msg.elements[i].type + msg.elements[i].position.x + msg.elements[i].position.y + msg.elements[i].size + msg.elements[i].value);\
            if (msg.elements[i].id != null && msg.elements[i].type != null && msg.elements[i].value != null && msg.elements[i].type != 'calendar') {\
                createElement(  cardId, \
                                msg.elements[i].type, \
                                msg.elements[i].position.x || 0, \
                                msg.elements[i].position.y || 0,\
                                msg.elements[i].size || 0,\
                                msg.elements[i].value\
                            );\
            }else{/*calendar*/\
                  createCalendar(  cardId, \
                                  msg.elements[i].type, \
                                  msg.elements[i].position.x || 0, \
                                  msg.elements[i].position.y || 0,\
                                  msg.elements[i].size || 0,\
                                  msg.elements[i].date.year || 1970,\
                                  msg.elements[i].date.month || 1,\
                                  msg.elements[i].date.day || 1,\
                                  msg.elements[i].position.align || 'left',\
	                                msg.elements[i].id\
                              );\
            }\
        }\
        publish('/cardResponse', {\
          cardId: cardId,\
          attention: msg.responseTopic,\
          action: 'create',\
          success: true\
        });\
	      selectCard('last');\
    }\
}
