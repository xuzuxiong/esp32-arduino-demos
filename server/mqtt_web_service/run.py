from dotenv import load_dotenv
from app import create_app
import os

load_dotenv()

app = create_app()

if __name__ == '__main__':
    port = int(os.getenv('FLASK_PORT', 8000))
    app.run(host='0.0.0.0', port=port, debug=os.getenv('FLASK_DEBUG', 'false').lower() == 'true')
