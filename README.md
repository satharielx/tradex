# TradeX

TradeX is a C++17 application that retrieves live cryptocurrency market depth data from Binance and maintains a thread-safe in-memory order book.

The project is built for Windows using native APIs and minimal external dependencies.

---

## Features

- Real-time market depth retrieval from Binance (REST API)
- Thread-safe order book implementation
- Windows-native networking via WinHTTP
- Modular, multi-file C++17 codebase
- Minimal dependencies

---

## Architecture

The application consists of the following main components:

- **WinHttpClient**  
  Handles HTTPS requests using the Windows WinHTTP API.

- **BinanceGateway**  
  Fetches market data from the Binance REST API, parses responses, and updates the order book.

- **OrderBook**  
  Stores bid and ask levels in a thread-safe structure and exposes best bid / ask information.

- **Logger**  
  Provides simple, thread-safe console logging.

---
## Requirements

- Windows 10 or newer
- Visual Studio 2026
- C++17 compatible compiler
- Internet connection

---

## Dependencies

- **WinHTTP** (built into Windows)
- **nlohmann.json** (installed via NuGet)

No additional package managers (vcpkg, Conan) are required.

---

## Build Instructions (Visual Studio)

1. Create a new **Console App (C++)** project named `TradeX`
2. Add all source files from the `src/` directory
3. Install the NuGet package:
   - nlohmann.json
4. Set the C++ language standard:
   - Project Properties → C/C++ → Language → ISO C++17
5. Build and run the project (x64 recommended)

---

## Usage

When run, TradeX continuously polls the Binance order book endpoint and prints the current best bid, best ask, and spread to the console.

Press **Enter** to stop the application.

---

## Notes

- Uses REST polling (not WebSockets)
- No trading or order execution logic
- No data persistence
- Subject to Binance API rate limits

---

## License

This project is provided as-is for educational and experimental purposes.

