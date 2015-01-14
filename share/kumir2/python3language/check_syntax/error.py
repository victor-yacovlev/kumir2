class Error:
    """ One error message """

    def __init__(self, line_no, start_pos, length, message):
        """
            line_no -- line number (int)
            start_pos -- start position from 0 (int)
            length -- error block length (int)
            message -- a string (ASCII symbols only) identificating error message
        """
        assert isinstance(line_no, int)
        assert isinstance(start_pos, int)
        assert isinstance(length, int)
        assert isinstance(message, str)
        self.line_no = line_no
        self.start_pos = start_pos
        self.length = length
        self.message = message
        self.origin = None

    def __eq__(self, other):
        return self.line_no == other.line_no and self.start_pos == other.start_pos