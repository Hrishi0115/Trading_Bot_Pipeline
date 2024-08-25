from env_vars.alpaca import api_key, secret_key

import asyncio
import websockets
import json

async def subscribe_to_alpaca(api_key, secret_key):
    url = "wss://stream.data.alpaca.markets/v1beta3/crypto/us"

    async with websockets.connect(url) as websocket:
        # Authenticate
        auth_message = {
            "action": "auth",
            "key": api_key,
            "secret": secret_key
        }

        await websocket.send(json.dumps(auth_message))
        response = await websocket.recv()
        print(f"Auth response: {response}")

        # subscribe to data

        subscribe_message = {
            "action": "subscribe",
            "quotes": ["BTC/USD"],
            "trades": ["BTC/USD"],
            "bars": []
        }

        await websocket.send(json.dumps(subscribe_message))
        response = await websocket.recv()
        print(f"Subscribe response: {response}")

        # Listen to incoming messages
        while True:
            message = await websocket.recv()
            print(f"Received message: {message}")


# Run the subscription
asyncio.get_event_loop().run_until_complete(subscribe_to_alpaca(api_key, secret_key))

# Received message: [{"T":"q","S":"BTC/USD","bp":64273.6,"bs":0.28016,"ap":64320.301,"as":0.55025616,"t":"2024-07-17T17:59:41.392732299Z"}, \
# {"T":"t","S":"BTC/USD","p":64320.301,"s":0.00045384,"t":"2024-07-17T17:59:41.392732299Z","i":908407144457061521,"tks":"B"}]