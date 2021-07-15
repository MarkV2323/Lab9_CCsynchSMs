tests = [ 
    
    { # Test 1: Should blink LED 0.
    'description': 'PINA: 0x00 (blinking requires no input) => PORTB: 0x01',
    'steps': [ {'inputs': [('PINA',0x00)], 'time': 500 },
               {'inputs': [('PINA',0x00)], 'time': 500 },
               ],
    'expected': [('PORTB',0x01)],
    },
    { # Test 2: Should blink LED 1 and 4
    'description': 'PINA: 0x00 (blinking requires no input) => PORTB: 0x0A',
    'steps': [ {'inputs': [('PINA',0x00)], 'time': 500 },
               {'inputs': [('PINA',0x00)], 'time': 500 },
               ],
    'expected': [('PORTB',0x0A)],
    },
    { # Test 2: Should blink LED 2
    'description': 'PINA: 0x00 (blinking requires no input) => PORTB: 0x04',
    'steps': [ {'inputs': [('PINA',0x00)], 'time': 500 },
               {'inputs': [('PINA',0x00)], 'time': 500 },
               ],
    'expected': [('PORTB',0x04)],
    },
    
]

watch = ['PORTB']
