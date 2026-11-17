#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
SES Host-Client: Beispiel fuer das USB-CDC-Binaerprotokoll der SES-Firmware.

Aufruf:
    python ses_client.py COM5              # Version + PD-Status abfragen
    python ses_client.py COM5 --measure    # Einzelmessung + Plot

Benoetigt: pyserial, numpy, matplotlib (nur fuer --measure)
"""
import argparse
import struct
import sys

import serial

MAGIC = b"\xA5\x5A"

RSP_VERSION     = 0x01
RSP_MEASUREMENT = 0x02
RSP_STREAM      = 0x03
RSP_PDSTATUS    = 0x04
RSP_ERROR       = 0x7F


def read_frame(ser: serial.Serial):
    """Frame lesen: Magic(2) Typ(1) Laenge(4 LE) Payload(n)."""
    while True:
        b = ser.read(1)
        if not b:
            raise TimeoutError("Keine Antwort vom Geraet")
        if b == MAGIC[0:1] and ser.read(1) == MAGIC[1:2]:
            break
    ftype = ser.read(1)[0]
    (length,) = struct.unpack("<I", ser.read(4))
    payload = ser.read(length)
    if len(payload) != length:
        raise TimeoutError("Frame unvollstaendig")
    return ftype, payload


def cmd_version(ser):
    ser.write(b"V")
    t, p = read_frame(ser)
    assert t == RSP_VERSION
    print(f"Firmware v{p[0]}.{p[1]}.{p[2]}")


def cmd_pdstatus(ser):
    ser.write(b"G")
    t, p = read_frame(ser)
    if t == RSP_ERROR:
        print("PD-Status nicht lesbar (STUSB4500 nicht erreichbar?)")
        return
    attached, dev_id = p[0], p[1]
    (rdo,) = struct.unpack("<I", p[2:6])
    (cur, maxcur) = struct.unpack("<HH", p[6:10])
    print(f"USB-C attached: {bool(attached)}, DeviceID: 0x{dev_id:02X}")
    print(f"RDO: 0x{rdo:08X}, Betriebsstrom: {cur} mA, max: {maxcur} mA")


def cmd_measure(ser, n_pulses=8, n_pairs=4096, plot=True):
    ser.write(b"M" + struct.pack("<HI", n_pulses, n_pairs))
    t, p = read_frame(ser)
    if t == RSP_ERROR:
        print("Messung fehlgeschlagen:", p.hex())
        return None
    assert t == RSP_MEASUREMENT
    (fs,) = struct.unpack("<I", p[0:4])
    (pulses,) = struct.unpack("<H", p[4:6])
    (pairs,) = struct.unpack("<I", p[6:10])
    print(f"Messung: fs={fs} Hz/Kanal, {pulses} Pulse, {pairs} Samplepaare")

    import numpy as np
    data = np.frombuffer(p[10:], dtype="<u2").reshape(-1, 2)
    left, right = data[:, 0], data[:, 1]
    t_ms = np.arange(pairs) / fs * 1e3
    dist_m = t_ms * 1e-3 * 343.0 / 2.0     # Laufzeit -> Entfernung

    if plot:
        import matplotlib.pyplot as plt
        fig, ax = plt.subplots(2, 1, sharex=True, figsize=(10, 6))
        ax[0].plot(t_ms, left, lw=0.5)
        ax[0].set_ylabel("ADC links [LSB]")
        ax[1].plot(t_ms, right, lw=0.5, color="tab:orange")
        ax[1].set_ylabel("ADC rechts [LSB]")
        ax[1].set_xlabel("Zeit nach Burst [ms]")
        ax2 = ax[0].secondary_xaxis(
            "top", functions=(lambda x: x * 1e-3 * 343.0 / 2.0,
                              lambda d: d * 2.0 / 343.0 * 1e3))
        ax2.set_xlabel("Entfernung [m]")
        fig.suptitle("SES Sonar-Einzelmessung")
        plt.tight_layout()
        plt.show()
    return data


def main():
    ap = argparse.ArgumentParser(description="SES USB-Host-Client")
    ap.add_argument("port", help="z. B. COM5 oder /dev/ttyACM0")
    ap.add_argument("--measure", action="store_true", help="Einzelmessung + Plot")
    ap.add_argument("--pulses", type=int, default=8)
    ap.add_argument("--pairs", type=int, default=4096)
    args = ap.parse_args()

    with serial.Serial(args.port, timeout=3) as ser:
        cmd_version(ser)
        cmd_pdstatus(ser)
        if args.measure:
            cmd_measure(ser, args.pulses, args.pairs)


if __name__ == "__main__":
    sys.exit(main())
