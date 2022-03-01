# handles the command history
import constants

class HistoryHandler():
    max_lines = constants.DEFAULT_MAX_LINES_HISTORY
    history = []
    history_print = ""

    def set_max_lines(self, new_max_lines:int):
        self.max_lines = new_max_lines

    def add_line(self, line:str, prefix="<-- "): # TODO tread every line as an object
        self.history.append(prefix+line)
        if len(self.history) > self.max_lines:
            self.history.pop(0)
        self.history_print = "".join(map(lambda x:x + "\n", self.history))

    def get_history_print(self):
        return self.history_print

