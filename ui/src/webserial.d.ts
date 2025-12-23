// ui/src/webserial.d.ts
// Minimal webserial typings for typescript to stop complaining istg
interface SerialPortRequestOptions {
  filters?: Array<{
    usbVendorId?: number;
    usbProductId?: number;
  }>;
}

interface SerialPortOpenOptions {
  baudRate: number;
  bufferSize?: number;
  dataBits?: 7 | 8;
  stopBits?: 1 | 2;
  parity?: "none" | "even" | "odd";
  flowControl?: "none" | "hardware";
}

interface SerialPort {
  readable?: ReadableStream<Uint8Array>;
  writable?: WritableStream<Uint8Array>;
  open(options: SerialPortOpenOptions): Promise<void>;
  close(): Promise<void>;
}

interface Serial {
  requestPort(options?: SerialPortRequestOptions): Promise<SerialPort>;
  getPorts(): Promise<SerialPort[]>;
}

interface Navigator {
  serial?: Serial;
}
