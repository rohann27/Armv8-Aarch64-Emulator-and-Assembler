import RPi.GPIO as GPIO
import time

# GPIO pin mapping (MSB to LSB)
LED_PINS = [2, 4, 17, 27]

# Set up GPIO
GPIO.setmode(GPIO.BCM)
for pin in LED_PINS:
    GPIO.setup(pin, GPIO.OUT)
    GPIO.output(pin, GPIO.LOW)

try:
    print("Starting LED display. Press Ctrl+C to stop.")
    while True:
        try:
            # Read digit from file
            with open('digit', 'r') as f:
                content = f.read().strip()
                
            if content.isdigit():
                digit = int(content)
                if 0 <= digit <= 15:
                    # Set LED states based on digit bits
                    for i in range(4):
                        bit_val = (digit >> (3 - i)) & 1
                        GPIO.output(LED_PINS[i], GPIO.HIGH if bit_val else GPIO.LOW)
                    print(f"Displaying digit {digit:>2} -> binary {digit:04b}")
                else:
                    print(f"Invalid digit {digit} (must be 0-15)")
            else:
                print(f"Invalid content in file: '{content}' (not a digit)")

        except FileNotFoundError:
            print("File 'digit' not found.")
        
        # Wait 0.5 seconds
        time.sleep(0.5)

except KeyboardInterrupt:
    print("\nStopping...")

finally:
    # Turn off LEDs and cleanup
    for pin in LED_PINS:
        GPIO.output(pin, GPIO.LOW)
    GPIO.cleanup()
    print("GPIO cleaned up.")
