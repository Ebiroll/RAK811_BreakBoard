# Viewing gps data with node red 

The mapper app have to be adjusted for the RAK811 payload format.

https://mydevices.com/cayenne/docs/lora/#lora-cayenne-low-power-payload
This is the app, modify your  "accessKey": "ttn-account-accessKEY"
The mapper part could also probably be replaced with something simpler...

[
    {
        "id": "7740b706.25b758",
        "type": "worldmap",
        "z": "85694fb8.ca277",
        "name": "Gateway",
        "lat": "59.42304736",
        "lon": "17.82984428",
        "zoom": "",
        "layer": "OSM",
        "cluster": "",
        "maxage": "",
        "usermenu": "show",
        "layers": "show",
        "panit": "false",
        "x": 700,
        "y": 240,
        "wires": []
    },
    {
        "id": "1be9e006.56db3",
        "type": "mapper",
        "z": "85694fb8.ca277",
        "name": "",
        "x": 420,
        "y": 240,
        "wires": [
            [
                "7740b706.25b758"
            ]
        ]
    },
    {
        "id": "87353fbb.34fb5",
        "type": "ttn message",
        "z": "85694fb8.ca277",
        "name": "mess",
        "app": "80a5700b.dd19c",
        "dev_id": "0x00012345",
        "field": "",
        "x": 230,
        "y": 160,
        "wires": [
            [
                "1be9e006.56db3"
            ]
        ]
    },
    {
        "id": "ef2eb3c8.f0ac7",
        "type": "debug",
        "z": "85694fb8.ca277",
        "name": "",
        "active": true,
        "console": "false",
        "complete": "true",
        "x": 690,
        "y": 140,
        "wires": []
    },
    {
        "id": "80a5700b.dd19c",
        "type": "ttn app",
        "z": "",
        "appId": "esp32_heltec",
        "region": "eu",
        "accessKey": "ttn-account-accessKEY"
    }
]

git clone https://github.com/Ebiroll/mapper

sudo npm link

cd ..

npm link mapper 


# More info

https://www.thethingsnetwork.org/docs/applications/nodered/quick-start.html

https://flows.nodered.org/node/node-red-contrib-web-worldmap

https://www.hackster.io/naresh-krish/integrate-node-red-with-rak811-lora-node-using-the-ttn-c4564a