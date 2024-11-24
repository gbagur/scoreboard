// pinout
#define D_BUZZER 23
#define BUZZER_PIN D_BUZZER

// constants
#define play playNote
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523

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

#define NOTE_1 NOTE_E4*4
#define NOTE_2 NOTE_G4*4
#define NOTE_3 NOTE_A4*4
#define NOTE_4 NOTE_B4*4
#define NOTE_5 NOTE_D5*4
#define NOTE_6 NOTE_DS5*4
#define NOTE_7 NOTE_E5*4


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

#define set      LedsON = HIGH; scoreSet();
#define clear    LedsON = LOW; scoreSet();

void buzzerDucks1() {
  int dur = 170;
  if (soundOn == LOW) {
    clear; delay(500); set; delay(500); clear; delay(500); set; delay(500); clear; delay(500); set; delay(500);
    return;
  }
  set
  playNoteAlways(NOTE_7, dur);
  clear
  playNoteAlways(NOTE_5, dur);
  set
  playNoteAlways(NOTE_4, dur);
  clear
  playNoteAlways(NOTE_3, dur);
    set
  playNoteAlways(NOTE_2, dur);
  clear
  playNoteAlways(NOTE_1, dur);
}

void buzzerDucks2() {
  int dur = 160;
  if (soundOn == LOW) {
    clear; delay(500); set; delay(500); clear; delay(500); set; delay(500); clear; delay(500); set; 
    return;
  }
  set
  playNoteAlways(NOTE_1, dur);
  clear
  playNoteAlways(NOTE_2, dur);
  set
  playNoteAlways(NOTE_3, dur);
  clear
  playNoteAlways(NOTE_4, dur);
  set
  playNoteAlways(NOTE_5, dur);
  clear
  playNoteAlways(NOTE_6, dur);
  set
  playNoteAlways(NOTE_7, dur*2);
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
  if (soundOn == LOW) {ledsBlink(); return;}
  if (frequency == NOTE_REST) {
    delay(duration);
  } else {
  tone(BUZZER_PIN, frequency, duration);
  delay(5); // Add a small delay to distinguish between notes
  }
}

void playNoteAlways(int frequency, int duration) {
  if (frequency == NOTE_REST) {
    delay(duration);
  } else {
  tone(BUZZER_PIN, frequency, duration);
  delay(5); // Add a small delay to distinguish between notes
  }
}


void buzzerClick() {
  playNote(NOTE_1, 30);
  playNote(NOTE_7, 30);
}

void buzzerSoundOn() {
  playNoteAlways(NOTE_1, 200);
  playNoteAlways(NOTE_7, 400);
}


void buzzerSoundOff ()  {
  playNoteAlways(NOTE_7, 200);
  playNoteAlways(NOTE_1, 400);
}


void buzzerBLEconnected() {
  playNoteAlways(NOTE_1, 50);
  playNoteAlways(NOTE_3, 50);
  playNoteAlways(NOTE_5, 50);
  playNoteAlways(NOTE_7, 50);
}

void buzzerBLEdisconnected() {
  playNoteAlways(NOTE_7, 50);
  playNoteAlways(NOTE_5, 50);
  playNoteAlways(NOTE_3, 50);
  playNoteAlways(NOTE_1, 50);
}

void buzzerStartMelody() {
  playNote(NOTE_1, Q);
  playNote(NOTE_2, Q);
  playNote(NOTE_3, Q);
  playNote(NOTE_4, Q);
  playNote(NOTE_5, Q);
  playNote(NOTE_6, Q);
  playNote(NOTE_7, H);
}


void buzzerChangeSide() {
  playNote(800, 500);
  delay(100);
  playNote(900, 500);
}

void playRest(int duration) {
  delay(duration);
}

void ledsBlink(void) {
    LedsON = !LedsON; scoreSet();
    delay(25);
    LedsON = !LedsON; scoreSet();
    delay(25);
    LedsON = !LedsON; scoreSet();
    delay(25);
    LedsON = !LedsON; scoreSet();
}