export type Packet =
  | { type: "telemetry"; env: number; rms: number; peak: number };

type Callbacks = {
  onStatus?: (s: string) => void;
  onLine?: (line: string) => void;
  onPacket?: (pkt: Packet) => void;
};

export class SerialLink {
  private port: SerialPort | null = null;
  private reader: ReadableStreamDefaultReader<Uint8Array> | null = null;
  private writer: WritableStreamDefaultWriter<Uint8Array> | null = null;
  private callbacks: Callbacks;

  private textDecoder = new TextDecoder();
  private textEncoder = new TextEncoder();
  private rxBuffer = "";

  constructor(callbacks: Callbacks) {
    this.callbacks = callbacks;
  }

  async connect() {
    if (!("serial" in navigator)) {
      throw new Error("WebSerial only works on Chrome or shitass Edge");
    }

    // @ts-ignore
    this.port = await navigator.serial.requestPort();

    // may need to change
    await this.port.open({ baudRate: 115200 });

    this.callbacks.onStatus?.("Connected");
    this.writer = this.port.writable?.getWriter() ?? null;
    this.reader = this.port.readable?.getReader() ?? null;

    this.readLoop().catch((e) => {
      this.callbacks.onStatus?.(`Disconnected (read error)`);
      this.callbacks.onLine?.(`[ui] read loop error: ${String(e)}`);
    });
  }

  async disconnect() {
    try {
      this.reader?.cancel();
      this.reader?.releaseLock();
      this.writer?.releaseLock();
      await this.port?.close();
    } finally {
      this.reader = null;
      this.writer = null;
      this.port = null;
      this.callbacks.onStatus?.("Disconnected");
    }
  }

  sendLine(line: string) { // basic text protocol for now
    this.sendBytes(this.textEncoder.encode(line + "\n"));
  }

  private sendBytes(bytes: Uint8Array) {
    if (!this.writer) return;
    this.writer.write(bytes);
  }

  private async readLoop() {
    if (!this.reader) return;

    while (true) {
      const { value, done } = await this.reader.read();
      if (done) break;
      if (!value) continue;

      this.rxBuffer += this.textDecoder.decode(value, { stream: true });

      // Process complete lines
      let idx: number;
      while ((idx = this.rxBuffer.indexOf("\n")) >= 0) {
        const line = this.rxBuffer.slice(0, idx).trim();
        this.rxBuffer = this.rxBuffer.slice(idx + 1);

        if (line.length === 0) continue;

        this.callbacks.onLine?.(line);

        // to change later!
        if (line.startsWith("TEL ")) {
          const pkt = parseTelemetryLine(line);
          if (pkt) this.callbacks.onPacket?.(pkt);
        }
      }
    }
  }
}

function parseTelemetryLine(line: string): Packet | null {
  // expected TEL env=0.123 rms=0.456 peak=0.789
  const parts = line.split(/\s+/);
  if (parts[0] !== "TEL") return null;

  const kv: Record<string, number> = {};
  for (let i = 1; i < parts.length; i++) {
    const [k, v] = parts[i].split("=");
    const n = Number(v);
    if (k && Number.isFinite(n)) kv[k] = n;
  }

  if (kv.env === undefined || kv.rms === undefined || kv.peak === undefined) return null;
  return { type: "telemetry", env: kv.env, rms: kv.rms, peak: kv.peak };
}
