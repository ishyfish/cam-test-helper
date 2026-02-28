from openai import OpenAI
from questions import QUESTION_BANK

class TranscriberAnalyzer:
    """
    Transcribes a WAV file using Whisper and analyzes
    the transcript with GPT for CAM delirium scoring.
    """

    def __init__(self, api_key, wav_path="C:\\Users\\tanto\\Downloads\\ECE198\\recordings\\audio.wav"):
        self.client = OpenAI(api_key=api_key)
        self.wav_path = wav_path

    def transcribe(self, question_id):
        print("📝 Transcribing audio with Whisper...")

        with open(self.wav_path, "rb") as audio_file:
            result = self.client.audio.transcriptions.create(
                model="whisper-1",
                file=audio_file,
                language = "en"
            )

        text = result.text

        question_text = QUESTION_BANK.get(question_id, "UNKNOWN QUESTION")

        formatted = f"Question ({question_id}): {question_text}\nAnswer: {text}\n"

        print(formatted)
        return formatted


    def analyze(self, text):
        print("🧠 Analyzing transcript with GPT (CAM scoring)...")

        prompt = f"""
        You are a clinician assistant at a hospital. Score this transcript for
        signs of delirium using CAM criteria. Ensure that any factual questions 
        (e.g., the current day, month, year, or location) are answered accurately 
        based on real-world information.

        Transcript:
        {text}

        Return JSON with:
        - attention_score (0–10)
        - orientation_score (0–10)
        - memory_score (0–10)
        - risk_level ("low", "moderate", "high")
        - short explanation
        """


        response = self.client.chat.completions.create(
            model="gpt-4.1",
            messages=[{"role": "user", "content": prompt}]
        )

        # ✅ Access content via attribute
        output = response.choices[0].message.content
        print("📊 CAM Assessment:\n", output)
        return output