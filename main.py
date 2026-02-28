from convert_raw_to_wav import RawToWavConverter
from transcribe_analyze import TranscriberAnalyzer
from questions import QUESTION_BANK
import os

API_KEY = 

TRANSCRIPT_PATH = "/Users/andrewma/vscode/camtester/recordings/interview.txt"
QUESTIONS = [
    "What day of the week is it?",
    "Count backward from 20 to 1.",
    "I am going to tell you three words: Apple, Table, Penny. Repeat them now.",
    "Do cats bark?"
]

def append_to_transcript(path, text):
    with open(path, "a", encoding="utf-8") as f:
        f.write(text + "\n")

def main():
    print("\n=== DELIRIUM AUDIO ANALYSIS PIPELINE ===\n")

    # Clear old transcript
    open(TRANSCRIPT_PATH, "w").close()
    processor = TranscriberAnalyzer(api_key=API_KEY)

    # You will receive exactly 4 audio files:
    # A1-x.raw, A2-x.raw, A3-x.raw, A4-x.raw
    for section in [1, 2, 3, 4]:

        # 🔍 Find the matching RAW file for this section (1-4)
        raw_file = f"/Users/andrewma/vscode/camtester/recordings/A{section}-"
        # Find real file name
        matched = None
        for fname in os.listdir("/Users/andrewma/vscode/ECE198/recordings"):
            if fname.startswith(f"A{section}-") and (fname.endswith(".raw") or fname.endswith(".RAW")):
                matched = fname
                break

        if matched is None:
            raise FileNotFoundError(f"No A{section}-*.raw file found!")

        raw_path = f"/Users/andrewma/vscode/camtester/recordings/{matched}"

        # Extract question ID like "1-3"
        question_id = matched.split(".")[0].replace("A", "")

        # 1) Add question to transcript
        question_text = QUESTION_BANK.get(question_id, "UNKNOWN QUESTION")

        # 2) Convert RAW → WAV (converter handles naming)
        converter = RawToWavConverter(raw_path=raw_path)
        wav_path = converter.convert()  # ensures WAV is created

        # 3) Transcribe after conversion
        processor.wav_path = wav_path
        formatted_block = processor.transcribe(question_id)

        # 4) Append transcribed block
        append_to_transcript(TRANSCRIPT_PATH, formatted_block + "\n")


    # Load full transcript and analyze
    with open(TRANSCRIPT_PATH, "r", encoding="utf-8") as f:
        full_text = f.read()

    print("\n=== FULL TRANSCRIPT ===")
    print(full_text)

    print("\n=== RUNNING CAM ANALYSIS ===")
    analysis = processor.analyze(full_text)

    print("\n=== FINISHED ===")


if __name__ == "__main__":
    main()