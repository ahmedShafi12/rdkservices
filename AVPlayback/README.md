-----------------
# AVPlayback

## Versions
`org.rdk.AVPlayback.1`

## Methods:
```
curl --header "Content-Type: application/json" --request POST --data '{"jsonrpc":"2.0", "id":3, "method":"org.rdk.AVPlayback.getVideoAspectRatio", "params":{}}' http://127.0.0.1:9998/jsonrpc
```
## Responses
```
{"jsonrpc":"2.0","id":3,"result":{"supportedVideoAspectRatio":["UNKNOWN", "4:3", "16:9"], "currentVideoAspectRatio":"16x9_FULL","success":true}}
```

## Events
```
videoAspectRatioChanged
```
## Events logged
```
videoAspectRatioChanged: Notify videoAspectRatioChanged {"url":"avpl://input0"}
```

## Full Reference
https://wiki.rdkcentral.com/display/RDK/AV+Playback

