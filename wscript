#!/usr/bin/env python

from waflib.Build import BuildContext
from waflib.TaskGen import feature, extension, after, task_gen
from waflib.Tools import ccroot

APPNAME = 'scribble'
VERSION = '0.01'

top = '.'
out = 'out'
apps = ['lcdshield', 'nokia']

_DISTROOT = '/Applications/Arduino.app/Contents/Resources/Java'
_HARDWARE = _DISTROOT + '/hardware'
_LIBRARIES = _DISTROOT + '/libraries'
_AVR = _HARDWARE + '/tools/avr'
_AVR_BIN = _HARDWARE + '/tools/avr/bin/%s'
_CORE_LIB = _HARDWARE + '/arduino/cores/arduino/'
_PLATFORM_LIB = _HARDWARE + '/arduino/variants/standard/'

_DIALECT = ['-Wall', '-Wextra', '-ansi', '-Wno-uninitialized', '-Wno-unused-parameter']

_BAUD = '115200'
_PROGRAMMER = 'arduino'

def configure(conf):
  conf.load('compiler_cxx')
  conf.env['PORT'] = conf.options.port
  conf.env['APP'] = conf.options.app

def options(opt):
  opt.load('compiler_cxx')
  opt.add_option('--device',
    default = 'atmega328p',
    action = 'store',
    help = 'the type of device we\'re building for')
  opt.add_option('--port',
    default = None,
    action = 'store',
    help = 'which port to flash (typically /dev/tty.usbmodem...')
  opt.add_option('--app',
    default = apps[0],
    action = 'store',
    help = 'which app to flash')
  opt.add_option('--clock',
    default = '16000000',
    action = 'store',
    help = 'clock frequency of the device')

# Wrapper around a waf builder thingy that adds some useful utilities.
# That way the nastiness is encapsulated here and the build rule can
# be cleaner.
class BuildHelper(object):

  _TOOLS = {
    'CXX': 'avr-g++',
    'LINK_CXX': 'avr-g++',
    'OBJCOPY': 'avr-objcopy',
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

  # Pulls the eeprom part out of the source binary and stores the rest
  # in a separate hex file.
  def remove_eeprom(self, source, target):
    self.build.new_task_gen(
      source = source,
      target = target,
      rule   = '${OBJCOPY} -O ihex -R .eeprom -R .fuse -R .lock ${SRC} ${TGT}'
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
    port = self.get_port()
    if not port:
      raise AssertionError("No --port specified.")
    self.build.new_task_gen(
      source  = progmem,
      depends = eeprom,
      rule    = '${AVRDUDE} -C%(avr)s/etc/avrdude.conf -v -v -v -v -p%(device)s -c%(programmer)s -P%(port)s -b%(baud)s -D -Uflash:w:${SRC}:i' % {
        'avr': _AVR,
        'device': self.get_device(),
        'port': port,
        'baud': _BAUD,
        'programmer': _PROGRAMMER
      },
      always  = True
    )

  def get_port(self):
    options = self.build.options
    if options.port:
      return options.port
    else:
      return self.build.env['PORT']
  
  def get_app(self):
    options = self.build.options
    if options.app:
      return options.app
    else:
      return self.build.env['APP']

  def get_clock(self):
    return self.build.options.clock

  def get_device(self):
    return self.build.options.device

  def build_sources(self):
    self.configure_toolchain()
    lib_sources = self.build.path.ant_glob('src/lib/**/*.(cc|cpp|c)')
    for app in apps:
      app_sources = ['src/%s.cc' % app]
      includes = [_CORE_LIB, _PLATFORM_LIB]
      elf_file = '%s.elf' % app
      self.program(
        source   = lib_sources + app_sources,
        target   = elf_file,
        includes = includes,
        cxxflags = ['-Os', '-mmcu=%s' % self.get_device()] + _DIALECT,
        linkflags = ['-Os', '-mmcu=%s' % self.get_device(), '-Wl,--gc-sections'],
        defines  = ['F_CPU=%sL' % self.get_clock()]
      )
      self.remove_eeprom(elf_file, '%s.hex' % app)
      self.extract_eeprom(elf_file, '%s.eep' % app)

def build(bld):
  helper = BuildHelper(bld)
  helper.build_sources()

def flash(bld):
  helper = BuildHelper(bld)
  app = helper.get_app()
  helper.build_sources()
  helper.flash('%s.hex' % app, '%s.eep' % app)

class FlashContext(BuildContext):
  cmd = 'flash'
  fun = 'flash'
