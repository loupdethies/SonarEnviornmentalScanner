#!/usr/bin/env python3
import os
import sys
import logging
import requests
from kipart import Kipart
from dotenv import load_dotenv

# Logging konfigurieren
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(message)s",
    datefmt="%Y-%m-%d %H:%M:%S"
)

# lädt key value Paare
load_dotenv()

# Mouser-API-Key aus Umgebungsvariable lesen
API_KEY = os.getenv("MOUSERSUCHE_API_KEY")
if not API_KEY:
    logging.error("Bitte Umgebungsvariable MOUSERSUCHE_API_KEY setzen!")
    sys.exit(1)

# Basis-URL der Mouser API
MOUSER_URL = "https://api.mouser.com/api/v1/search/partnumber"

def fetch_part_info(part_number, version="1"):
    """
    Ruft die Mouser-API /search/partnumber auf und
    gibt das erste Suchergebnis als Dict zurück.
    """
    url = f"https://api.mouser.com/api/v{version}/search/partnumber"
    headers = {
        "Accept": "application/json",
        "Content-Type": "application/json",
    }
    payload = {
        "SearchByPartRequest": {
            "mouserPartNumber": part_number,
            "partSearchOptions": ""
        }
    }

    logging.info(f"Sende POST an Mouser: {part_number}")
    resp = requests.post(
        url,
        params={"apiKey": API_KEY},
        headers=headers,
        json=payload,
        timeout=10
    )
    resp.raise_for_status()
    data = resp.json()
    
## Debug-Ausgabe (temporär)
    # import json
    # print(json.dumps(data, indent=2))

    # Fehlerbehandlung
    if data.get("Errors"):
        err = data["Errors"][0]
        msg = f"Mouser-API Error {err['Code']}: {err['Message']}"
        logging.error(msg)
        raise RuntimeError(msg)

    # neues Parsing:
    parts = data.get("SearchResults", {}).get("Parts", [])
        
    if not parts:
        raise RuntimeError(f"Kein Ergebnis für {part_number}")
    return parts[0]   # erstes Ergebnis zurückgeben
    
def import_to_kicad(part, sym_lib, fp_lib):
    kip = Kipart()
    kip.add(
        part_number_mouser = part["MouserPartNumber"],
        symbol_library=sym_lib,
        footprint_library=fp_lib,
        footprint="Capacitor_SMD:C_0603_1608Metric",
        part_number = part["ManufacturerPartNumber"],
        manufacturer = part["Manufacturer"],
        description = part["Description"]
    )
    kip.write_symbol_library()

# Beispiel-Aufruf
if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    part = "810-C1608X7R1H104K"
    SymbolLib = "MyLib.kicad_sym"
    FootprintLib = "MyFootprints.pretty"
    info = fetch_part_info(part)
    import_to_kicad(info, SymbolLib, FootprintLib) 
    print("Hersteller:", info["Manufacturer"])
    print("Datasheet-URL:", info.get("DataSheetUrl"))
    print("Beschreibung:", info["Description"])
