import serial
import pandas as pd
from datetime import datetime


porta_serial = 'COM3' # mude para a porta serial do seu computador
baudrate = 115200
intervalo_ms = 500

dados = []
ultimo_log = None
ultima_amostragem = datetime.min


def log_diferente(novo_log, ultimo_log):
    return ultimo_log is None or novo_log != ultimo_log


# Abre a porta serial
ser = serial.Serial(porta_serial, baudrate, timeout=1)
nome_arquivo = 'serial_data.csv'

try:
    print(f"Monitoring serial door {porta_serial}... Press Ctrl+C to stop.")
    while True:
        if ser.in_waiting > 0:
            linha = ser.readline().decode('utf-8').strip()
            timestamp = datetime.now()


            if (timestamp - ultima_amostragem).total_seconds() * 1000 >= intervalo_ms and log_diferente(linha, ultimo_log):
                dados.append([timestamp.strftime('%Y-%m-%d %H:%M:%S'), linha])
                print(f'{timestamp.strftime("%H:%M:%S")} - {linha}')

                ultimo_log = linha
                ultima_amostragem = timestamp

except KeyboardInterrupt:
    print("\nSerial reading interupted!")
finally:
    ser.close()
    df = pd.DataFrame(dados, columns=['Timestamp', 'Data'])
    df.to_csv(nome_arquivo, index=False)
    print(f'Data sucessfuly saved in {nome_arquivo}')