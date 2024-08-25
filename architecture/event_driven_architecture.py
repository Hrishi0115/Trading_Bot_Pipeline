import asyncio
import yfinance as yf
import numpy as np
from collections import deque
from datetime import datetime
from alpaca.trading.client import TradingClient
from alpaca.trading.requests import MarketOrderRequest
from alpaca.trading.enums import OrderSide, TimeInForce
from env_vars.alpaca import api_key, secret_key
import logging

# Set up logging
logging.basicConfig(filename='trading_strategy.log', level=logging.INFO, format='%(asctime)s:%(levelname)s:%(message)s')

# Input parameters
ticker_symbol = "AAPL"
short_window_length = 40
long_window_length = 100

# Initialize trading client
trading_client = TradingClient(api_key=api_key, secret_key=secret_key, paper=True)

# Initialize strategy
data = yf.download(ticker_symbol, period="1d", interval="1m")
long_window = deque(data['Adj Close'].tail(long_window_length), maxlen=long_window_length)
short_window = deque(data['Adj Close'].tail(short_window_length), maxlen=short_window_length)

initial_long_mavg = np.mean(long_window)
initial_short_mavg = np.mean(short_window)
prev_short_mavg = initial_short_mavg
prev_long_mavg = initial_long_mavg

logging.info("Initialization finished")

async def fetch_latest_price():
    ticker = yf.Ticker(ticker_symbol)
    price = ticker.fast_info['lastPrice']
    return price

async def trade():
    global prev_short_mavg, prev_long_mavg

    try:
        latest_price = await fetch_latest_price()
        latest_short_mavg, latest_long_mavg = update_moving_averages(latest_price, short_window, long_window)

        order_type = None
        order_id = None

        if latest_short_mavg > latest_long_mavg and prev_short_mavg <= prev_long_mavg:
            order_type = 'bought'
            market_order_data = MarketOrderRequest(symbol=ticker_symbol, qty=100, side=OrderSide.BUY, time_in_force=TimeInForce.IOC)
            market_order = trading_client.submit_order(order_data=market_order_data)
            order_id = market_order.id

        elif latest_short_mavg < latest_long_mavg and prev_short_mavg >= prev_long_mavg:
            order_type = 'sold'
            try:
                position = trading_client.get_open_position(ticker_symbol)
                qty = position.qty
                market_order_data = MarketOrderRequest(symbol=ticker_symbol, qty=qty, side=OrderSide.SELL, time_in_force=TimeInForce.IOC)
                market_order = trading_client.submit_order(order_data=market_order_data)
                order_id = market_order.id
            except Exception as e:
                logging.error(f"Error fetching position or submitting sell order: {e}")

        if order_id:
            order = trading_client.get_order_by_id(order_id)
            if order.status == 'filled':
                logging.info(f"Order executed: {order.qty} shares of {order.symbol} {order_type} at ${order.filled_avg_price}")
            else:
                logging.info(f"Order status: {order.status}")
        else:
            logging.info("No trade executed.")

        prev_short_mavg = latest_short_mavg
        prev_long_mavg = latest_long_mavg

    except Exception as e:
        logging.error(f"An error occurred: {e}")

def update_moving_averages(latest_price, short_window, long_window):
    short_window.append(latest_price)
    long_window.append(latest_price)
    short_mavg = np.mean(short_window)
    long_mavg = np.mean(long_window)
    return short_mavg, long_mavg

async def main():
    while True:
        await trade()
        await asyncio.sleep(60)

# Run the main function

asyncio.run(main())