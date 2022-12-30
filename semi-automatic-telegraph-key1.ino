// ***************************************************************
// Symbol playing buffer
// length of buffer must be a power of 2

#define buff_len 8
#define buff_idx_mask  (buff_len - 1)
unsigned char sym_play_buff[buff_len];
unsigned char sym_play_wr = 0;
unsigned char sym_play_rd = 0;

// function returns either symbol from sym_play_buff or NULL if there is no data in the buffer
char * read_sym(void)
{
    char * psym = NULL;
    if (sym_play_wr != sym_play_rd)
    {
        psym = &sym_play_buff[sym_play_rd];
        sym_play_rd++;
        sym_play_rd &= buff_idx_mask;
    }
    return psym;
}

void write_sym(char sym)
{
    sym_play_buff[sym_play_wr] = sym;
    sym_play_wr++;
    sym_play_wr &= buff_idx_mask;
}

typedef enum
{
    st_none = 0,
    st_dot  = 1,
    st_dash = 2,
    st_pause = 3
}symbol_t;

unsigned char last_sym;

#define KEY_DOT  6
#define KEY_DASH 7
#define BUZZER   3

//RED
#define LED_TX 1
#define LED_TX_ON 0
#define LED_TX_OFF 1

// GREEEN
#define LED_RX 0
#define LED_RX_ON 0
#define LED_RX_OFF 1

//RED
#define LED_L 13
#define LED_L_ON 1
#define LED_L_OFF 0

int val = 0;
unsigned int dot_duration = 150;
unsigned int last_duration = 0;
char last_sound_state = 0;
char amplitude = 0;
char last_played_sym = st_none;

void play_buff(void)
{
    if (last_duration)
    {
        if (amplitude != 0)
        {
            last_sound_state = 1 - last_sound_state;
            if (last_sound_state)
            {
                PORTD = B00000100;
            }else{
                PORTD = B00000000;
            }
        }
        last_duration--;
        if ((last_duration == 0) && (amplitude == 0))
        {
            last_played_sym = st_pause;
        }

    }
    else
    {
        char * sym = read_sym();
        if (sym != NULL)
        {
            if (*sym == st_dot)
            {
                    last_duration = dot_duration;
                    last_played_sym = st_dot;
                    amplitude = 1;
            }
            if (*sym == st_dash)
            {
                    last_duration = 3*dot_duration;
                    last_played_sym = st_dash;
                    amplitude = 1;
            }
            if (*sym == st_pause)
            {
                last_duration = (dot_duration+dot_duration/2);
                //last_played_sym = st_pause;
                amplitude = 0;
            }
        }
        else
        {
            if (last_sym != st_none)
            {
                    write_sym(last_sym);
                    write_sym(st_pause);
                    last_sym = st_none;
            }
        }
    }
}

void read_key(void)
{
    val = PINB;
    if (val & B00001000)
    {
        digitalWrite(LED_TX,  LED_TX_OFF);
    } else {
        digitalWrite(LED_TX,  LED_TX_ON);
        if (last_played_sym != st_dot)
        {
            last_played_sym = st_dot;
            last_sym = st_dot;
        }
    }

    if (val & B00010000)
    {
        digitalWrite(LED_L,  LED_L_OFF);
    } else {
        digitalWrite(LED_L,  LED_L_ON);
        if (last_played_sym != st_dash)
        {
            last_played_sym = st_dash;
            last_sym = st_dash;
        }
    }
}

void setup()
{
    DDRD = B11111100;

    DDRB =  B00000100;
    PORTB = B00000000;
    //pinMode(KEY_DOT,   OUTPUT);
    //pinMode(KEY_DOT, INPUT_PULLUP);
    //pinMode(KEY_DASH, INPUT_PULLUP);
    pinMode(LED_TX, OUTPUT);
    pinMode(LED_L,  OUTPUT);
}

void loop()
{
    read_key();
    play_buff();
    //digitalWrite(KEY_DOT, HIGH);
    //digitalWrite(KEY_DOT, LOW);

    // PORTD = B00000100;
    delayMicroseconds(1200);
    // PORTD = B00000000;
    // delayMicroseconds(1500);
}

/*



//PWR
//PWR LED is always ON

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  //pinMode(PIN, OUTPUT);
  pinMode(LED_L, OUTPUT);
  pinMode(LED_TX, OUTPUT);
  pinMode(LED_RX, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_TX, LED_TX_ON);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED_L,  LED_L_ON);
  digitalWrite(LED_RX,  LED_RX_ON);
  delay(1000);                       // wait for a second
  digitalWrite(LED_TX, LED_TX_OFF);    // turn the LED off by making the voltage LOW
  digitalWrite(LED_L,  LED_L_OFF);
  digitalWrite(LED_RX, LED_RX_OFF);
  delay(1000);                       // wait for a second
}
*/
