// generic card functions
exports.imgRootPath = '/usr/bin/img'; // This should be relative to the card? unless there are shared images?

// general helper functions
exports.zeroPad = function (num, places) {
    var zero = places - num.toString().length + 1;
    return Array(+(zero > 0 && zero)).join("0") + num;
}

// time&date functions
exports.getEpochMillis = function () {
    return Date.now();
}
 
exports.generateDate = function () {
    var dt = new Date
    return {
        'type': 'calendar',
        'value': 'update',
        'id': 0,
        'date': {
            'year': dt.getFullYear(),
            'month': dt.getMonth() + 1,
            'day': dt.getDate()
        }
    };
}


exports.getEpoch = function () {
    return Math.floor(cardLib.getEpochMillis() / 1000);
}

function getCurrentHour() {
    var d = new Date();
    return d.getHours();
}

// path generating functions
exports.generateImgPath = function (rootPath, imgName) {
    return rootPath + '/img_' + imgName + '.bin';
}

// functions generating card objects
exports.generateNewCardObj = function (bgColor, responseTopic) {
    return {
        'cmd': 'new_card',
        'bg_color': bgColor,    // TODO: change bgColor from hex to dec
        'responseTopic': responseTopic,
        'elements': []
    };
}

exports.generateUpdateCardObj = function (cardId) {
    return {
        'cmd': 'update_card',
        'id': cardId,
        'elements': []
    };
}

function generateElement(id, type, value, posX, posY, alignment) {
    var obj = {
        'id': id,
        'type': type,
        'value': value
    }

    if (typeof posX !== 'undefined' && typeof posY !== 'undefined') {
        var pos = {
            'x': posX,
            'y': posY
        };
        if (typeof alignment !== 'undefined' &&
            (alignment === 'left' || alignment === 'center' || alignment === 'right')
        ) {
            pos['align'] = alignment;
        }
        obj['position'] = pos;
    }

    return obj;
}

exports.generateElementCal = function (id, type, value, posX, posY, alignment) {
    var dt = new Date
    var obj = {
        'id': id,
        'type': type,
        'size': value,
        'date': {
            'year': dt.getFullYear(),
            'month': dt.getMonth() + 1,
            'day': dt.getDate()
        }
    }

    if (typeof posX !== 'undefined' && typeof posY !== 'undefined') {
        var pos = {
            'x': posX,
            'y': posY
        };
        if (typeof alignment !== 'undefined' &&
            (alignment === 'left' || alignment === 'center' || alignment === 'right')
        ) {
            pos['align'] = alignment;
        }
        obj['position'] = pos;
    }

    return obj;
}

exports.generateTextElement = function (id, value, size, posX, posY, alignment) {
    var obj = generateElement(id, 'text', value, posX, posY, alignment);

    if (size !== null) {
        obj['size'] = size;
    }

    return obj;
}

exports.generateImageElement = function (id, value, posX, posY, alignment) {
    return generateElement(id, 'image', value, posX, posY, alignment);
}

exports.generateElementUpdate = function (id, value) {
    return {
        'id': id,
        'value': value
    };
}

// message queue functions 
// code review: good to have this function since every card requires this action?
exports.onRecvMessage = function (e) {
    if (typeof e.topic !== 'undefined' && typeof e.payload !== 'undefined') {
        var payloadObj = e.payload;

        if (typeof e.payload === 'string' || e.payload instanceof String) {
            print('recvMessage: payload is a string, converting...');
            // print('recvMessage: ' + e.payload);
            try {
                payloadObj = JSON.parse(e.payload);
            } catch (err) {
                print(err); // error in the above string (in this case, yes)!
                print('ERROR Could not parse payload')
                return null;
            }
        }

        if (e.topic === '/button' || e.topic === '/gesture') {
            onInput({
                source: e.topic.replace(/^\//, ''),
                payload: payloadObj
            });
        } else {
            onMessage({
                topic: e.topic,
                payload: payloadObj
            });
        }

    }
}

exports.handleCardResponseMessage = function (cardInfo, payload) {
    if (typeof payload === 'string' || payload instanceof String) {
        try {
            payload = JSON.parse(payload);
        } catch (e) {
            print(e); // error in the above string (in this case, yes)!
            print('ERROR Could not parse payload')
            return null;
        }
    }
    if (typeof payload.cardId !== 'undefined' && typeof payload.action !== 'undefined') {
        switch (payload.action) {
            case 'create':
                if (cardInfo.id < 0 &&
                    typeof payload.attention !== 'undefined' &&
                    payload.attention === cardInfo.responseTopic) {
                    // assign the card it's id
                    cardInfo.id = payload.cardId;
                    print('Assigning card its id: ' + cardInfo.id);
                    setCardNightlightColors(cardInfo.nightlight || 0);
                }
                break;

            case 'select':
                if (cardInfo.id === payload.cardId) {
                    // this has become the active card
                    print('Card has become active: ' + cardInfo.id);
                    cardInfo.active = true;
                    setCardNightlightColors(cardInfo.nightlight || 0);
                } else {
                    // another card has become the active card
                    cardInfo.active = false;
                    print('A different card has become active');
                }
                break;
            case 'update':
                if (payload.success === false && payload.reason && payload.reason === 'invalid cardId') {
                    cardInfo.invalid = true;
                    print('Card identified as invalid');
                }
                break;
            default:
                break;
        }
    }
    return cardInfo;
}

function setCardNightlightColors(color) {
    if (Array.isArray(color)) {
        // color.forEach(function(element, id) {
        //   //print('setting button ' + id + ' to ' + element); 
        //   setButton(id, element);
        // });
        var payload = {
            cmd: 'buttons',
            value: color
        }
        publish('/set', JSON.stringify(payload));
    } else {
        setNightLight(color);
    }
}
