from dotenv import load_dotenv
import os

# load environment variables from .env file

load_dotenv()

api_key = os.getenv('ALPACA_API_KEY')
secret_key = os.getenv('ALPACA_SECRET_KEY')