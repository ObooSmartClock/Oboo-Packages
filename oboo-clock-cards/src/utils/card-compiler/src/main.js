function setup() {
  var card = {
    "cmd":"new_card",
    "bg_color": 13,
    "responseTopic": "/card_new1"
  }

  card.elements = [
    {
      "id": 0,
      "type": "image",
      "position": {
        "x": 194,
        "y": 22
      },
      "value": "img/img_degC.bin"
    },
    {
      "id": 1,
      "type": "text",
      "position": {
        "x": 130,
        "y": 44,
        "align": "right"
      },
      "size": 80,
      "value": "0"
    },
    {
      "id": 2,
      "type": "text",
      "position": {
        "x": 117,
        "y": 142,
        "align": "right"
      },
      "size": 30,
      "value": "'`date '+%A'`'"
    },
    {
      "id": 3,
      "type": "text",
      "position": {
        "x": 142,
        "y": 173,
        "align": "right"
      },
      "size": 30,
      "value": "'`date '+%b %d'`'"
    },
    {
      "id": 4,
      "type": "text",
      "position": {
        "x": 100,
        "y": 220,
        "align": "right"
      },
      "size": 20,
      "value": "Precipitation"
    },
    {
      "id": 5,
      "type": "text",
      "position": {
        "x": 115,
        "y": 236,
        "align": "right"
      },
      "size": 60,
      "value": "10%"
    },
    {
      "id": 6,
      "type": "image",
      "position": {
        "x": 10,
        "y": 30
      },
      "value": "img/img_sunny.bin"
    }
    ,
    {
      "id": 7,
      "type": "image",
      "position": {
        "x": 25,
        "y": 240
      },
      "value": "img/img_percipitation.bin"
    }
  ]


  console.log(opt.cmd);
  console.log(opt.bg_color);
  // console.log(opt);

  initCard(card)
}

function loop() {
  sleep(10)

}
