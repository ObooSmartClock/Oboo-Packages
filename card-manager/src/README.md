# Oboo-Clock-Card-Manager
Display and Card manager for the Oboo Clock running on the Omega2

# Notifications

Display Manager listens to `/notification` topic.

## Setting a Notification

To set a notification, send a message to `/notification` with the following syntax:

```
{
  "cmd": "set",
  "text": "<NOTIFICATION TEXT>",
  "size": <FONT SIZE>       // OPTIONAL: default is 20
}
```

## Clearing a Notification

To clear a notification, send a message to `/notification` like the following:

```
{
  "cmd": "clear"
}
```

# Status Icons

Display manager listens to the `/status` topic

## Setting a Status Icon

To configure a status icon, publish a message to `/status` like the following:

```
{ 
  "cmd":"update", 
  "elements": [ 
    { 
      "type": "<STATUS TYPE>",    // string
      "value": <STATUS VALUE>     // integer representing the status 
    } 
  ] 
}
```

## Available Status Icons

On and off icons, can be set to `0` for off, or `1` for on:

* `wifi`
* `bluetooth`
* `alarm`

Range icons:

* `battery`
  * Accepts range from `0` (empty battery) to `4` (full battery)


