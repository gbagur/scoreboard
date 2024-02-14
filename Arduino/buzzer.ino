#include led.h

#define play playNote
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523

#define BUZZER_PIN 18

#define NOTE_C4   262
#define NOTE_CS4  277
#define NOTE_D4   294
#define NOTE_DS4  311
#define NOTE_E4   330
#define NOTE_F4   349
#define NOTE_FS4  370
#define NOTE_G4   392
#define NOTE_GS4  415
#define NOTE_A4   440
#define NOTE_AS4  466
#define NOTE_B4   494
#define NOTE_C5   523
#define NOTE_CS5  554
#define NOTE_D5   587
#define NOTE_DS5  622
#define NOTE_E5   659
#define NOTE_F5   698
#define NOTE_FS5  740
#define NOTE_G5   784
#define NOTE_GS5  831
#define NOTE_A5   880
#define NOTE_AS5  932
#define NOTE_B5   988
#define NOTE_C6   1047
#define NOTE_CS6  1109
#define NOTE_D6   1175
#define NOTE_REST 9999

// Define the duration of each note
#define BPM 250
#define A 4 * 60000 / BPM // whole note
#define H 2 * 60000 / BPM // half note
#define Q 60000 / BPM // quarter note
#define E 60000 / (BPM * 2) // eighth note
#define S 60000 / (BPM * 4) // sixteenth note


// Define the duration of each note
#define TEMPO 150
#define DURATION_QUARTER  TEMPO
#define DURATION_EIGHTH   (TEMPO / 2)
#define DURATION_HALF     (TEMPO * 2)



void buzzerInit(){
  // Initialize the buzzer pin as an output
  pinMode(BUZZER_PIN, OUTPUT);
}
void buzzerPlayMelodyEndGame() {
  // Play Twinkle Twinkle Little Star
  playNote(NOTE_C4, DURATION_QUARTER);
  playNote(NOTE_C4, DURATION_QUARTER);
  playNote(NOTE_G4, DURATION_QUARTER);
  playNote(NOTE_G4, DURATION_QUARTER);
  playNote(NOTE_A4, DURATION_QUARTER);
  playNote(NOTE_A4, DURATION_QUARTER);
  playNote(NOTE_G4, DURATION_HALF);

  playNote(NOTE_F4, DURATION_QUARTER);
  playNote(NOTE_F4, DURATION_QUARTER);
  playNote(NOTE_E4, DURATION_QUARTER);
  playNote(NOTE_E4, DURATION_QUARTER);
  playNote(NOTE_D4, DURATION_QUARTER);
  playNote(NOTE_D4, DURATION_QUARTER);
  playNote(NOTE_C4, DURATION_HALF);
}

#define setScore      scoreSet(SideTeamA, scoreTeamA); scoreSet(SideTeamB, scoreTeamB);
#define clearScore    scoreSet(SideTeamA, 100); scoreSet(SideTeamB, 100);

void buzzerDucks() {

  setScore
  playNote(NOTE_G4, S);
  playNote(NOTE_G4, S);
  clearScore
  playNote(NOTE_A4, S);
  playNote(NOTE_A4, S);
  setScore
  playNote(NOTE_E4, S);
  playNote(NOTE_E4, S);
  clearScore
  playNote(NOTE_G4, S);
  playRest(S);
  setScore
  playNote(NOTE_G4, S);
  playNote(NOTE_G4, S);
  clearScore
  playNote(NOTE_A4, S);
  playNote(NOTE_A4, S);
  setScore
  playNote(NOTE_E4, S);
  playNote(NOTE_E4, S);
  clearScore
  playNote(NOTE_G4, S);
  playRest(S);
  setScore
  // playNote(NOTE_G4, S);
  // playNote(NOTE_G4, S);
  // clearScore
  // playNote(NOTE_A4, S);
  // playNote(NOTE_A4, S);
  // setScore
  // playNote(NOTE_G4, S);
  // playNote(NOTE_G4, S);
  // playNote(NOTE_B4, Q);
}
void buzzerPlayMelodyMario() {
  // Super Mario Bros Theme Melody
  play(NOTE_E5, E);
  play(NOTE_E5, E);
  play(NOTE_REST, E);
  play(NOTE_E5, E);
  play(NOTE_REST, E);
  play(NOTE_C5, E);
  play(NOTE_E5, Q);
  play(NOTE_REST, E);
  play(NOTE_G5, Q);
  play(NOTE_REST, Q);
  play(NOTE_G4, Q);

  play(NOTE_C5, Q);
  play(NOTE_G4, E);
  play(NOTE_E4, E);
  play(NOTE_A4, Q);
  play(NOTE_B4, Q);
  play(NOTE_AS4, A);
  play(NOTE_A4, E);
  play(NOTE_GS4, E);

  play(NOTE_G4, E);
  play(NOTE_E5, Q);
  play(NOTE_A5, Q);
  play(NOTE_REST, Q);
  play(NOTE_FS5, E);
  play(NOTE_F5, E);
  play(NOTE_E5, E);
  play(NOTE_DS5, E);

  play(NOTE_B4, Q);
  play(NOTE_C5, E);
  play(NOTE_REST, E);
  play(NOTE_G4, Q);
  play(NOTE_REST, Q);
  play(NOTE_E4, Q);
  play(NOTE_REST, Q);
  play(NOTE_A4, Q);

  play(NOTE_B4, E);
  play(NOTE_AS4, A);
  play(NOTE_A4, E);
  play(NOTE_GS4, E);
  play(NOTE_G4, E);
  play(NOTE_DS5, Q);
  play(NOTE_B4, E);
  play(NOTE_AS4, A);

  play(NOTE_A4, E);
  play(NOTE_GS4, E);
  play(NOTE_G4, E);
  play(NOTE_E5, Q);
  play(NOTE_A5, Q);
  play(NOTE_REST, Q);
  play(NOTE_FS5, E);

  play(NOTE_F5, E);
  play(NOTE_E5, E);
  play(NOTE_DS5, E);
  play(NOTE_B4, Q);
  play(NOTE_C5, E);
  play(NOTE_REST, E);
  play(NOTE_G4, Q);
  play(NOTE_REST, Q);

  play(NOTE_E4, Q);
  play(NOTE_REST, Q);
  play(NOTE_A4, Q);
  play(NOTE_B4, E);
  play(NOTE_AS4, A);
  play(NOTE_A4, E);
  play(NOTE_GS4, E);
  play(NOTE_G4, E);

  play(NOTE_E5, Q);
  play(NOTE_A5, Q);
  play(NOTE_REST, Q);
  play(NOTE_FS5, E);
  play(NOTE_F5, E);
  play(NOTE_E5, E);
  play(NOTE_DS5, E);
  play(NOTE_B4, Q);

  play(NOTE_C5, E);
  play(NOTE_REST, E);
  play(NOTE_G4, Q);
  play(NOTE_REST, Q);
  play(NOTE_E4, Q);
  play(NOTE_REST, Q);
  play(NOTE_C6, Q);
  play(NOTE_C6, Q);
  play(NOTE_C6, Q);
  delay(1000); // Pause between repeats
}

void playNote(int frequency, int duration) {
  if (frequency == NOTE_REST) {
    delay(duration);
  } else {
  tone(BUZZER_PIN, frequency, duration);
  delay(5); // Add a small delay to distinguish between notes
  }
}

void buzzerClick() {
  playNote(NOTE_C6, 50);
}



void buzzerChangeSide() {
  playNote(NOTE_F4, 500);
  delay(100);
  playNote(NOTE_F4, 500);
}

void playRest(int duration) {
  delay(duration);
}

