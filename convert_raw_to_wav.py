import os
import wave
import numpy as np


class RawToWavConverter:
    """
    Converts unsigned 8-bit PCM raw audio into a proper WAV file.
    """

    def __init__(self, raw_path, wav_path=None, sample_rate=43000):
        self.raw_path = raw_path
        if wav_path is None:
            self.wav_path = raw_path.rsplit('.', 1)[0] + ".wav"
        else:
            self.wav_path = wav_path
        self.sample_rate = sample_rate

    def convert(self):
        if not os.path.exists(self.raw_path):
            raise FileNotFoundError(f"RAW file not found: {self.raw_path}")

        print(f"🔄 Converting RAW → WAV: {self.raw_path}")

        raw_data = np.fromfile(self.raw_path, dtype=np.uint8)
        audio_data = ((raw_data.astype(np.int16) - 128) * 256)  # 8-bit → 16-bit PCM

        with wave.open(self.wav_path, "wb") as wav_file:
            wav_file.setnchannels(1)
            wav_file.setsampwidth(2)
            wav_file.setframerate(self.sample_rate)
            wav_file.writeframes(audio_data.tobytes())

        print(f"✅ WAV file created: {self.wav_path}")
        return self.wav_path
