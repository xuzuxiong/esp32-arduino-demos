"""Load environment: .env first, then .env.example placeholders if missing."""
from pathlib import Path
import os

from dotenv import load_dotenv


def load_app_env() -> None:
    root = Path(__file__).resolve().parent
    load_dotenv(root / ".env")
    # Open-source default: fall back to committed placeholders when .env is absent.
    if not os.getenv("SECRET_KEY"):
        load_dotenv(root / ".env.example")
