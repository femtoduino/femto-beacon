import machine, time
from machine import Timer
from Femto import HSVColors

global colors
colors = HSVColors(26, 4, 5)

tim = Timer(-1)
tim.init(period=100, mode=Timer.PERIODIC, callback=lambda t: colors.rotate())

while True:
    state = machine.disable_irq()
    machine.enable_irq(state)
    time.sleep_ms(100)