class FormatTokenPrinter:
  def __init__(self, val):
      self.val = val

  def to_string (self):
      return self.val['TokenText']['Data']

class StringRefPrinter:
  def __init__(self, val):
    self.val = val

  def display_hint(self):
    return "string"

  def to_string(self):
    return ''.join(chr((self.val['Data'] + x).dereference()) for x in range(int(self.val["Length"])))