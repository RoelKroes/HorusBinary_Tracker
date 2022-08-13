volatile bool execute = false;

void setup_timer(uint16_t ticks)
{
  // Pause interrupts
  cli();

  // Reset counter1
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
 
  // Set the value that the counter has to reach before it triggers an interrupt
  // 15624 = 1s (assuming you use 1024 as the prescaler value)
  // Counter1 is 16-bit so the value must be less than or equal to 65535
  OCR1A = ticks;
 
  // Clear timer on compare match
  // The timer resets itself when it reaches 15625 (OCR1A +1)
  TCCR1B |= (1 << WGM12);
 
  // Set the prescaler to 1024 (See ATMega328PU datasheet for infos)
  // TCCR1B |= (1 << CS12) | (1 << CS10); // prescaler = 1024
  TCCR1B |= (1 << CS12); // prescaler = 256
  // Enable timer interrupt
  TIMSK1 |= (1 << OCIE1A);

  // Enable interrupts
  sei();
}

ISR(TIMER1_COMPA_vect)
{
  // The MCU calls this function whenever the timer resets itself
  // (i.e., once every second).
 
  execute = true;
}
