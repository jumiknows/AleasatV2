# handles the command history
import constants
from typing import List

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

class CommandHistoryHandler():
    history = []
    history_index = 0

    def __init__(self, history:List[str]):
        self.history = history.copy()

    def save_input(self, input_text:str):
        self.history.insert(0, input_text)
        self.history_index = 0
    
    def get_previous_command(self) -> str:
        if self.history == []:
            return ""
        if self.history_index > 0:
            self.history_index -= 1
        return self.history[self.history_index]
    
    def get_next_command(self) -> str:
        if self.history_index < len(self.history):
            self.history_index += 1
        return self.history[self.history_index-1]
