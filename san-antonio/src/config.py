import configparser, ast
from typing import List

sanantoniorc = ".san_antoniorc"
command_history_heading = "command_history"
history_heading = "history"
command_split = "\\bleh\\"

class ConfigHandler():
    def __init__(self) -> None:
        self.config = configparser.ConfigParser()
        self.config.read(sanantoniorc)
    
    def get_command_history(self)->List[str]:
        if self.config.has_section(command_history_heading):
            return self.config.get(command_history_heading, history_heading).split(command_split)
        else:
            self.config.add_section(command_history_heading)
            return []

    def update(self, commands_list:List[str]):
        str_commands_list = command_split.join(commands_list)
        self.config.set(command_history_heading, history_heading, str_commands_list)
        return

    def save(self):
        with open(sanantoniorc, 'w') as configfile:
            self.config.write(configfile)