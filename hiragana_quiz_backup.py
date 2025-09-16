import sys
import random
from PyQt5.QtWidgets import (
    QApplication, QWidget, QLabel, QLineEdit, QPushButton, QVBoxLayout, QMessageBox, QCheckBox, QHBoxLayout, QTableWidget, QTableWidgetItem, QHeaderView
)
from PyQt5.QtCore import Qt


# Hiragana, Katakana, and basic Kanji to Romaji mapping
HIRAGANA_ROMAJI = [
    ("あ", "a"), ("い", "i"), ("う", "u"), ("え", "e"), ("お", "o"),
    ("か", "ka"), ("き", "ki"), ("く", "ku"), ("け", "ke"), ("こ", "ko"),
    ("さ", "sa"), ("し", "shi"), ("す", "su"), ("せ", "se"), ("そ", "so"),
    ("た", "ta"), ("ち", "chi"), ("つ", "tsu"), ("て", "te"), ("と", "to"),
    ("な", "na"), ("に", "ni"), ("ぬ", "nu"), ("ね", "ne"), ("の", "no"),
    ("は", "ha"), ("ひ", "hi"), ("ふ", "fu"), ("へ", "he"), ("ほ", "ho"),
    ("ま", "ma"), ("み", "mi"), ("む", "mu"), ("め", "me"), ("も", "mo"),
    ("や", "ya"), ("ゆ", "yu"), ("よ", "yo"),
    ("ら", "ra"), ("り", "ri"), ("る", "ru"), ("れ", "re"), ("ろ", "ro"),
    ("わ", "wa"), ("を", "wo"), ("ん", "n")
]

KATAKANA_ROMAJI = [
    ("ア", "a"), ("イ", "i"), ("ウ", "u"), ("エ", "e"), ("オ", "o"),
    ("カ", "ka"), ("キ", "ki"), ("ク", "ku"), ("ケ", "ke"), ("コ", "ko"),
    ("サ", "sa"), ("シ", "shi"), ("ス", "su"), ("セ", "se"), ("ソ", "so"),
    ("タ", "ta"), ("チ", "chi"), ("ツ", "tsu"), ("テ", "te"), ("ト", "to"),
    ("ナ", "na"), ("ニ", "ni"), ("ヌ", "nu"), ("ネ", "ne"), ("ノ", "no"),
    ("ハ", "ha"), ("ヒ", "hi"), ("フ", "fu"), ("ヘ", "he"), ("ホ", "ho"),
    ("マ", "ma"), ("ミ", "mi"), ("ム", "mu"), ("メ", "me"), ("モ", "mo"),
    ("ヤ", "ya"), ("ユ", "yu"), ("ヨ", "yo"),
    ("ラ", "ra"), ("リ", "ri"), ("ル", "ru"), ("レ", "re"), ("ロ", "ro"),
    ("ワ", "wa"), ("ヲ", "wo"), ("ン", "n")
]

# A small set of basic Kanji and their readings
KANJI_ROMAJI = [
    ("日", "nichi"),  # sun, day
    ("月", "getsu"),  # moon, month
    ("火", "ka"),     # fire
    ("水", "sui"),    # water
    ("木", "moku"),   # tree
    ("金", "kin"),    # gold
    ("土", "do"),     # earth
    ("山", "yama"),   # mountain
    ("川", "kawa"),   # river
    ("人", "hito"),   # person
    ("口", "kuchi"),  # mouth
    ("目", "me"),     # eye
    ("耳", "mimi"),   # ear
    ("手", "te"),     # hand
    ("足", "ashi"),   # foot
    ("力", "chikara") # power
]


class AlphabetQuiz(QWidget):
    def highlight_char_in_tables(self, char, romaji, color):
        # Helper to highlight the character in the top tables by changing font color
        def highlight_in_table(table, alphabet, char, romaji, color):
            per_row = 5
            for idx, (c, r) in enumerate(alphabet):
                if c == char and r == romaji:
                    row = idx // per_row
                    col = idx % per_row + 1
                    item = table.item(row, col)
                    if item:
                        if color == 'green':
                            item.setForeground(Qt.green)
                        elif color == 'red':
                            item.setForeground(Qt.red)
                        else:
                            item.setForeground(Qt.black)
        highlight_in_table(self.hiragana_table, HIRAGANA_ROMAJI, char, romaji, color)
        highlight_in_table(self.katakana_table, KATAKANA_ROMAJI, char, romaji, color)
        highlight_in_table(self.kanji_table, KANJI_ROMAJI, char, romaji, color)

    def reset_all_table_highlights(self):
        def reset_table(table, alphabet):
            per_row = 5
            for idx, (c, r) in enumerate(alphabet):
                row = idx // per_row
                col = idx % per_row + 1
                item = table.item(row, col)
                if item:
                    if table.isEnabled():
                        item.setForeground(Qt.white)
        reset_table(self.hiragana_table, HIRAGANA_ROMAJI)
        reset_table(self.katakana_table, KATAKANA_ROMAJI)
        reset_table(self.kanji_table, KANJI_ROMAJI)

    def __init__(self):
        super().__init__()
        self.setWindowTitle("Japanese Alphabet Quiz")
        self.setGeometry(100, 100, 1200, 700)
        self.correct_count = 0
        self.retry_count = 0
        self.retry_queue = []
        self.last_wrong = None
        self.char_weights = {}  # (char, romaji): weight
        self.answered_once = set()  # (char, romaji) that have been answered correctly at least once
        self.char_stats = {}  # (char, romaji): {'shown': int, 'correct': int, 'incorrect': int}
        self.times_to_show = 2  # Default: show each character once per round
        self.init_ui()
        self.new_question()

    def init_ui(self):
        self.layout = QVBoxLayout()

        # Tables for Hiragana, Katakana, Kanji with row checkboxes
        self.tables_layout = QHBoxLayout()
        self.hiragana_table, self.hiragana_row_checks = self.create_alphabet_table_with_checkboxes(HIRAGANA_ROMAJI, "Hiragana")
        self.katakana_table, self.katakana_row_checks = self.create_alphabet_table_with_checkboxes(KATAKANA_ROMAJI, "Katakana")
        self.kanji_table, self.kanji_row_checks = self.create_alphabet_table_with_checkboxes(KANJI_ROMAJI, "Kanji")
        self.hiragana_table.setMinimumWidth(350)
        self.katakana_table.setMinimumWidth(350)
        self.kanji_table.setMinimumWidth(350)
        self.hiragana_table.setMaximumHeight(350)
        self.katakana_table.setMaximumHeight(350)
        self.kanji_table.setMaximumHeight(350)
        self.tables_layout.addWidget(self.hiragana_table)
        self.tables_layout.addWidget(self.katakana_table)
        self.tables_layout.addWidget(self.kanji_table)
        self.layout.addLayout(self.tables_layout)

        # Checkboxes for script selection
        self.checkbox_layout = QHBoxLayout()
        self.hiragana_cb = QCheckBox("Hiragana", self)
        self.hiragana_cb.setChecked(True)
        self.hiragana_cb.stateChanged.connect(lambda state: self.handle_script_checkbox('hiragana', state))
        self.checkbox_layout.addWidget(self.hiragana_cb)

        self.katakana_cb = QCheckBox("Katakana", self)
        self.katakana_cb.setChecked(True)
        self.katakana_cb.stateChanged.connect(lambda state: self.handle_script_checkbox('katakana', state))
        self.checkbox_layout.addWidget(self.katakana_cb)

        self.kanji_cb = QCheckBox("Kanji", self)
        self.kanji_cb.setChecked(True)
        self.kanji_cb.stateChanged.connect(lambda state: self.handle_script_checkbox('kanji', state))
        self.checkbox_layout.addWidget(self.kanji_cb)

        self.layout.addLayout(self.checkbox_layout)

        # (Times to show each character per round) UI removed as per request

        # Score label
        self.score_label = QLabel("Correct: 0 | Retries: 0", self)
        self.score_label.setAlignment(Qt.AlignCenter)
        self.score_label.setStyleSheet("font-size: 18px; font-weight: bold;")
        self.layout.addWidget(self.score_label)

        self.char_label = QLabel("", self)
        self.char_label.setAlignment(Qt.AlignCenter)
        self.char_label.setStyleSheet("font-size: 64px;")
        self.layout.addWidget(self.char_label)

        self.input = QLineEdit(self)
        self.input.setPlaceholderText("Enter Romaji...")
        self.input.returnPressed.connect(self.check_answer)
        self.layout.addWidget(self.input)

        # Bottom message label
        self.bottom_label = QLabel("", self)
        self.bottom_label.setAlignment(Qt.AlignCenter)
        self.bottom_label.setStyleSheet("font-size: 16px; color: red;")
        self.layout.addWidget(self.bottom_label)

        self.setLayout(self.layout)

    def handle_script_checkbox(self, script, state):
        # Helper to get table and row_checks by script name
        if script == 'hiragana':
            table = self.hiragana_table
        elif script == 'katakana':
            table = self.katakana_table
        elif script == 'kanji':
            table = self.kanji_table
        else:
            return
        block = True if state == 0 else False
        # Enable/disable table
        table.setDisabled(block)
        self.new_question()

    def create_alphabet_table_with_checkboxes(self, alphabet, title):
        from PyQt5.QtWidgets import QCheckBox, QWidget, QHBoxLayout
        from PyQt5.QtGui import QFont
        per_row = 5
        n = len(alphabet)
        rows = (n + per_row - 1) // per_row
        table = QTableWidget()
        table.setColumnCount(per_row + 1)  # +1 for checkbox
        table.setRowCount(rows)
        table.horizontalHeader().setVisible(False)
        table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        table.verticalHeader().setVisible(False)
        font = QFont()
        font.setPointSize(12)
        row_checks = []

        def make_row_cb_handler(cb, row_checks):
            def handler(state):
                if state == 0:
                    # If this is the last checked box, revert
                    checked_count = sum(1 for c in row_checks if c.isChecked())
                    if checked_count == 0:
                        cb.blockSignals(True)
                        cb.setChecked(True)
                        cb.blockSignals(False)
                        return
                self.new_question()
            return handler

        for row in range(rows):
            cb = QCheckBox()
            cb.setChecked(True)
            cb.stateChanged.connect(make_row_cb_handler(cb, row_checks))
            row_checks.append(cb)
            widget = QWidget()
            layout = QHBoxLayout(widget)
            layout.addWidget(cb)
            layout.setAlignment(cb, Qt.AlignCenter)
            layout.setContentsMargins(0, 0, 0, 0)
            widget.setLayout(layout)
            table.setCellWidget(row, 0, widget)
        # Fill table with char (romaji)
        for idx, (char, romaji) in enumerate(alphabet):
            row = idx // per_row
            col = idx % per_row + 1  # +1 for checkbox
            item = QTableWidgetItem(f"{char} ({romaji})")
            item.setFont(font)
            table.setItem(row, col, item)
        table.setEditTriggers(QTableWidget.NoEditTriggers)
        table.setSelectionMode(QTableWidget.NoSelection)
        table.setSizeAdjustPolicy(QTableWidget.AdjustToContents)
        table.resizeColumnsToContents()
        table.resizeRowsToContents()
        return table, row_checks

    def update_score(self):
        self.score_label.setText(f"Correct: {self.correct_count} | Retries: {self.retry_count}")

    def get_enabled_alphabets(self):
        enabled = []
        # Helper to get checked rows for a table
        def get_checked_rows(row_checks, alphabet):
            per_row = 5
            n = len(alphabet)
            rows = (n + per_row - 1) // per_row
            chars = []
            for row in range(rows):
                if row_checks[row].isChecked():
                    # Add all chars in this row
                    for i in range(per_row):
                        idx = row * per_row + i
                        if idx < n:
                            chars.append(alphabet[idx])
            return chars
        if self.hiragana_cb.isChecked():
            enabled.append(get_checked_rows(self.hiragana_row_checks, HIRAGANA_ROMAJI))
        if self.katakana_cb.isChecked():
            enabled.append(get_checked_rows(self.katakana_row_checks, KATAKANA_ROMAJI))
        if self.kanji_cb.isChecked():
            enabled.append(get_checked_rows(self.kanji_row_checks, KANJI_ROMAJI))
        # Initialize weights for all enabled chars if not already present
        all_chars = [item for sublist in enabled for item in sublist]
        for char in all_chars:
            if char not in self.char_weights:
                self.char_weights[char] = 1.0
        # Remove weights for chars not enabled
        to_remove = [k for k in self.char_weights if k not in all_chars]
        for k in to_remove:
            del self.char_weights[k]
            self.answered_once.discard(k)
        return enabled

    def new_question(self, exclude_current=False):
        self.bottom_label.setText("")
        enabled = self.get_enabled_alphabets()
        if not enabled:
            self.char_label.setText("")
            QMessageBox.warning(self, "No Script Selected", "Please select at least one script to practice.")
            self.input.setDisabled(True)
            return
        else:
            self.input.setDisabled(False)
        # Flatten the enabled lists
        all_chars = [item for sublist in enabled for item in sublist]
        # Build a list of chars to show, each repeated self.times_to_show times
        round_chars = []
        for c in all_chars:
            round_chars.extend([c] * self.times_to_show)
        # Remove those already answered self.times_to_show times
        char_counts = {}
        for c in self.answered_once:
            char_counts[c] = char_counts.get(c, 0) + 1
        unanswered_chars = []
        for c in round_chars:
            if char_counts.get(c, 0) < self.times_to_show:
                unanswered_chars.append(c)
                char_counts[c] = char_counts.get(c, 0) + 1
        # If all have been answered the required number of times, show summary and reset
        if not unanswered_chars:
            self.show_summary_and_reset(all_chars)
            return
        # Exclude the current character if requested and possible
        current_tuple = getattr(self, 'current_char', None), getattr(self, 'current_romaji', None)
        if exclude_current and len(unanswered_chars) > 1:
            unanswered_chars = [c for c in unanswered_chars if c != current_tuple]
        # Randomly select from unanswered_chars
        chosen = random.choice(unanswered_chars)
        self.current_char, self.current_romaji = chosen
        self.last_wrong = None
        # Update stats for shown
        char_tuple = (self.current_char, self.current_romaji)
        if char_tuple not in self.char_stats:
            self.char_stats[char_tuple] = {'shown': 0, 'correct': 0, 'incorrect': 0}
        self.char_stats[char_tuple]['shown'] += 1
        self.char_label.setText(self.current_char)
        self.input.clear()
        # Only highlight after user attempts an answer, not when question is shown

    # handle_times_dropdown removed as per request

    def show_summary_and_reset(self, all_chars):
        # Count right and wrong
        right = 0
        wrong = 0
        hard_chars = []
        for char_tuple in all_chars:
            stats = self.char_stats.get(char_tuple, {'correct': 0, 'incorrect': 0})
            if stats['incorrect'] > 0:
                wrong += 1
                if stats['incorrect'] > 1:
                    hard_chars.append(f"{char_tuple[0]} ({char_tuple[1]})")
            else:
                right += 1
        msg = f"Quiz Complete!\n\nCorrect: {right}\nIncorrect: {wrong}"
        if hard_chars:
            msg += "\n\nYou should work on these characters (missed more than once):\n"
            msg += "\n".join(hard_chars)
        else:
            msg += "\n\nGreat job!"
        QMessageBox.information(self, "Quiz Summary", msg)
        # Reset for next round
        self.reset_all_table_highlights()
        self.answered_once.clear()
        self.char_stats.clear()
        self.char_weights.clear()
        self.retry_queue.clear()
        self.correct_count = 0
        self.retry_count = 0
        self.update_score()
        self.new_question()

    def check_answer(self):
        from PyQt5.QtCore import QTimer
        user_input = self.input.text().strip().lower()
        self.bottom_label.setText("")
        char_tuple = (self.current_char, self.current_romaji)
        if char_tuple not in self.char_stats:
            self.char_stats[char_tuple] = {'shown': 1, 'correct': 0, 'incorrect': 0}
        # Highlight the character in the top tables only when user attempts an answer
        if user_input == self.current_romaji:
            self.highlight_char_in_tables(self.current_char, self.current_romaji, 'green')
            self.correct_count += 1
            self.answered_once.add(char_tuple)
            # Reduce weight, but not to zero
            if char_tuple in self.char_weights:
                self.char_weights[char_tuple] = max(self.char_weights[char_tuple] * 0.2, 0.01)
            self.char_stats[char_tuple]['correct'] += 1
            self.update_score()
            # self.add_to_correct_table(char_tuple)
            # self.remove_from_incorrect_table(char_tuple)
            self.new_question()
        else:
            self.highlight_char_in_tables(self.current_char, self.current_romaji, 'red')
            self.retry_count += 1
            self.char_stats[char_tuple]['incorrect'] += 1
            self.update_score()
            self.bottom_label.setText(f"Incorrect! {self.current_char} = {self.current_romaji}")
            # Add to retry queue if not already present
            if char_tuple not in self.retry_queue:
                self.retry_queue.append(char_tuple)
            # self.add_to_incorrect_table(char_tuple)
            self.flash_char_label_red_then_next()

    def remove_from_incorrect_table(self, char_tuple):
        for row in range(self.incorrect_table.rowCount()):
            if self.incorrect_table.item(row, 0) and self.incorrect_table.item(row, 0).text() == f"{char_tuple[0]} ({char_tuple[1]})":
                self.incorrect_table.removeRow(row)
                break

    def flash_char_label_red_then_next(self):
        from PyQt5.QtCore import QTimer
        flashes = ["color: red; font-size: 64px;", "color: black; font-size: 64px;"] * 3
        def do_flash():
            if flashes:
                self.char_label.setStyleSheet(flashes.pop(0))
                QTimer.singleShot(170, do_flash)
            else:
                self.char_label.setStyleSheet("font-size: 64px;")
                QTimer.singleShot(50, lambda: self.new_question(exclude_current=True))  # Ensure new_question runs after the event loop processes
        do_flash()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = AlphabetQuiz()
    window.show()
    sys.exit(app.exec_())
