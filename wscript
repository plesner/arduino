#!/usr/bin/env python

from waflib.Build import BuildContext
from waflib.TaskGen import feature, extension, after, task_gen
from waflib.Tools import ccroot

APPNAME = 'arduino'
VERSION = '0.01'

top = '.'
out = 'out'

_AVR = '/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr'
_HARDWARE = '/Applications/Arduino.app/Contents/Resources/Java/hardware'
_AVR_BIN = _HARDWARE + '/tools/avr/bin/%s'
_CORE_LIB = _HARDWARE + '/arduino/cores/arduino/'
_PLATFORM_LIB = _HARDWARE + '/arduino/variants/standard/'
_DEVICE = 'atmega328p'
_CLOCK = '16000000L'

def configure(conf):
  conf.load('compiler_cxx')

def options(opt):
  opt.load('compiler_cxx')

# Wrapper around a waf builder thingy that adds some useful utilities.
# That way the nastiness is encapsulated here and the build rule can
# be cleaner.
class BuildHelper(object):

  _TOOLS = {
    'CXX': 'avr-g++', 'LINK_CXX': 'avr-g++', 'OBJCOPY': 'avr-objcopy',
    'AVRDUDE': 'avrdude'
  }

  def __init__(self, build):
    self.build = build

  # Defines a program; just passes the call on to the builder.
  def program(self, **kwds):
    self.build.program(**kwds)

  # Overrides the default toolchain with the AVR one.
  def configure_toolchain(self):
    for name, tool in BuildHelper._TOOLS.items():
      self.build.env[name] = _AVR_BIN % tool

  # Copies a core library from the arduino distribution to the build
  # directory so waf will put the compiled object files there, rather
  # than pollute the distribution.
  #
  # Returns the name of the file in the build directory.
  def copy_core_file(self, name):
    core_name = 'core_%s' % name
    self.build.new_task_gen(
      source = self.build.root.find_resource(_CORE_LIB + name),
      target = core_name,
      rule   = 'cp ${SRC} ${TGT}'
    )
    return core_name

  # Pulls the eeprom part out of the source binary and stores the rest
  # in a separate hex file.
  def remove_eeprom(self, source, target):
    self.build.new_task_gen(
      source = source,
      target = target,
      rule   = '${OBJCOPY} -O ihex -R .eeprom ${SRC} ${TGT}'
    )

  # Pulls the eeprom part out of the source binary and stores it in a
  # separate hex file.
  def extract_eeprom(self, source, target):
    flags = "-O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0"
    self.build.new_task_gen(
      source = source,
      target = target,
      rule   = '${OBJCOPY} %s ${SRC} ${TGT}' % flags
    )

  def flash(self, progmem, eeprom):
    self.build.new_task_gen(
      source  = progmem,
      depends = eeprom,
      rule    = '${AVRDUDE} -C%s/etc/avrdude.conf -v -v -v -v -patmega328p -carduino -P/dev/tty.usbmodem621 -b115200 -D -Uflash:w:${SRC}:i' % _AVR,
      always  = True
    )

def build(bld):
  helper = BuildHelper(bld)
  helper.configure_toolchain()
  user_srcs = bld.path.ant_glob('src/**/*.cc')
  core_srcs = [helper.copy_core_file(s) for s in [
    'main.cpp',
    'wiring_digital.c',
    'wiring.c'
  ]]
  includes = [_CORE_LIB, _PLATFORM_LIB]
  helper.program(
    source   = user_srcs + core_srcs,
    target   = 'blinky.elf',
    includes = includes,
    cxxflags = ['-Os', '-mmcu=%s' % _DEVICE],
    linkflags = ['-Os', '-mmcu=%s' % _DEVICE],
    defines  = ['F_CPU=%s' % _CLOCK]
  )
  helper.remove_eeprom('blinky.elf', 'blinky.hex')
  helper.extract_eeprom('blinky.elf', 'blinky.eep')

def flash(bld):
  build(bld)
  helper = BuildHelper(bld)
  helper.flash('blinky.hex', 'blinky.eep')

class FlashContext(BuildContext):
  cmd = 'flash'
  fun = 'flash'
