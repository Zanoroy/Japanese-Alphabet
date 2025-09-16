import sys
import random
from PyQt5.QtWidgets import (
    QApplication, QWidget, QLabel, QLineEdit, QPushButton, QVBoxLayout, QMessageBox, QCheckBox, QHBoxLayout, QTableWidget, QTableWidgetItem, QHeaderView
)
from PyQt5.QtCore import Qt


# Hiragana, Katakana, and basic Kanji to Romaji mapping
HIRAGANA_ROMAJI = [
    # a to o
    (("あ", "a"), ("い", "i"), ("う", "u"), ("え", "e"), ("お", "o")),
    # ka to ko
    (("か", "ka"), ("き", "ki"), ("く", "ku"), ("け", "ke"), ("こ", "ko")),
    # sa to so
    (("さ", "sa"), ("し", "shi"), ("す", "su"), ("せ", "se"), ("そ", "so")),
    # ta to to
    (("た", "ta"), ("ち", "chi"), ("つ", "tsu"), ("て", "te"), ("と", "to")),
    # na to no
    (("な", "na"), ("に", "ni"), ("ぬ", "nu"), ("ね", "ne"), ("の", "no")),
    # ha to ho
    (("は", "ha"), ("ひ", "hi"), ("ふ", "fu"), ("へ", "he"), ("ほ", "ho")),
    # ma to mo
    (("ま", "ma"), ("み", "mi"), ("む", "mu"), ("め", "me"), ("も", "mo")),
    # ya row
    (("や", "ya"), ("", ""), ("ゆ", "yu"), ("", ""), ("よ", "yo")),
    # ra row
    (("ら", "ra"), ("り", "ri"), ("る", "ru"), ("れ", "re"), ("ろ", "ro")),
    # wa row
    (("わ", "wa"), ("", ""), ("", ""), ("", ""), ("を", "wo")),
    # n row
    (("ん", "n"), ("", ""), ("", ""), ("", ""), ("", "")),
    # dakuon (voiced)
    (("が", "ga"), ("ぎ", "gi"), ("ぐ", "gu"), ("げ", "ge"), ("ご", "go")),
    (("ざ", "za"), ("じ", "ji"), ("ず", "zu"), ("ぜ", "ze"), ("ぞ", "zo")),
    (("だ", "da"), ("ぢ", "ji"), ("づ", "zu"), ("で", "de"), ("ど", "do")),
    (("ば", "ba"), ("び", "bi"), ("ぶ", "bu"), ("べ", "be"), ("ぼ", "bo")),
    # handakuon (p)
    (("ぱ", "pa"), ("ぴ", "pi"), ("ぷ", "pu"), ("ぺ", "pe"), ("ぽ", "po")),
    # # yōon (combinations)
    # (("きゃ", "kya"), ("", ""), ("きゅ", "kyu"), ("", ""), ("きょ", "kyo")),
    # (("しゃ", "sha"), ("", ""), ("しゅ", "shu"), ("", ""), ("しょ", "sho")),
    # (("ちゃ", "cha"), ("", ""), ("ちゅ", "chu"), ("", ""), ("ちょ", "cho")),
    # (("にゃ", "nya"), ("", ""), ("にゅ", "nyu"), ("", ""), ("にょ", "nyo")),
    # (("ひゃ", "hya"), ("", ""), ("ひゅ", "hyu"), ("", ""), ("ひょ", "hyo")),
    # (("みゃ", "mya"), ("", ""), ("みゅ", "myu"), ("", ""), ("みょ", "myo")),
    # (("りゃ", "rya"), ("", ""), ("りゅ", "ryu"), ("", ""), ("りょ", "ryo")),
    # (("ぎゃ", "gya"), ("", ""), ("ぎゅ", "gyu"), ("", ""), ("ぎょ", "gyo")),
    # (("じゃ", "ja"), ("", ""), ("じゅ", "ju"), ("", ""), ("じょ", "jo")),
    # (("びゃ", "bya"), ("", ""), ("びゅ", "byu"), ("", ""), ("びょ", "byo")),
    # (("ぴゃ", "pya"), ("", ""), ("ぴゅ", "pyu"), ("", ""), ("ぴょ", "pyo")),
]

KATAKANA_ROMAJI = [
    # a to o
    (("ア", "a"), ("イ", "i"), ("ウ", "u"), ("エ", "e"), ("オ", "o")),
    # ka to ko
    (("カ", "ka"), ("キ", "ki"), ("ク", "ku"), ("ケ", "ke"), ("コ", "ko")),
    # sa to so
    (("サ", "sa"), ("シ", "shi"), ("ス", "su"), ("セ", "se"), ("ソ", "so")),
    # ta to to
    (("タ", "ta"), ("チ", "chi"), ("ツ", "tsu"), ("テ", "te"), ("ト", "to")),
    # na to no
    (("ナ", "na"), ("ニ", "ni"), ("ヌ", "nu"), ("ネ", "ne"), ("ノ", "no")),
    # ha to ho
    (("ハ", "ha"), ("ヒ", "hi"), ("フ", "fu"), ("ヘ", "he"), ("ホ", "ho")),
    # ma to mo
    (("マ", "ma"), ("ミ", "mi"), ("ム", "mu"), ("メ", "me"), ("モ", "mo")),
    # ya row
    (("ヤ", "ya"), (""  , "" ), ("ユ", "yu"), (""  , "" ), ("ヨ", "yo")),
    # ra row
    (("ラ", "ra"), ("リ", "ri"), ("ル", "ru"), ("レ", "re"), ("ロ", "ro")),
    # wa row
    (("ワ", "wa"), (""  , "" ), (""  , "" ), (""  , "" ), ("ヲ", "wo")),
    # n row
    (("ン", "n"), (""  , "" ), (""  , "" ), (""  , "" ), (""  , "" )),
    # dakuon (voiced)
    (("ガ", "ga"), ("ギ", "gi"), ("グ", "gu"), ("ゲ", "ge"), ("ゴ", "go")),
    (("ザ", "za"), ("ジ", "ji"), ("ズ", "zu"), ("ゼ", "ze"), ("ゾ", "zo")),
    (("ダ", "da"), ("ヂ", "ji"), ("ヅ", "zu"), ("デ", "de"), ("ド", "do")),
    (("バ", "ba"), ("ビ", "bi"), ("ブ", "bu"), ("ベ", "be"), ("ボ", "bo")),
    # handakuon (p)
    (("パ", "pa"), ("ピ", "pi"), ("プ", "pu"), ("ペ", "pe"), ("ポ", "po")),
    # # yōon (combinations)
    # (("キャ", "kya"), ("", ""), ("キュ", "kyu"), ("", ""), ("キョ", "kyo")),
    # (("シャ", "sha"), ("", ""), ("シュ", "shu"), ("", ""), ("ショ", "sho")),
    # (("チャ", "cha"), ("", ""), ("チュ", "chu"), ("", ""), ("チョ", "cho")),
    # (("ニャ", "nya"), ("", ""), ("ニュ", "nyu"), ("", ""), ("ニョ", "nyo")),
    # (("ヒャ", "hya"), ("", ""), ("ヒュ", "hyu"), ("", ""), ("ヒョ", "hyo")),
    # (("ミャ", "mya"), ("", ""), ("ミュ", "myu"), ("", ""), ("ミョ", "myo")),
    # (("リャ", "rya"), ("", ""), ("リュ", "ryu"), ("", ""), ("リョ", "ryo")),
    # (("ギャ", "gya"), ("", ""), ("ギュ", "gyu"), ("", ""), ("ギョ", "gyo")),
    # (("ジャ", "ja"), ("", ""), ("ジュ", "ju"), ("", ""), ("ジョ", "jo")),
    # (("ビャ", "bya"), ("", ""), ("ビュ", "byu"), ("", ""), ("ビョ", "byo")),
    # (("ピャ", "pya"), ("", ""), ("ピュ", "pyu"), ("", ""), ("ピョ", "pyo")),
]

# A small set of basic Kanji and their readings
KANJI_ROMAJI = [
    (("日", "nichi"),  # sun, day
    ("月", "getsu"),  # moon, month
    ("火", "ka"),     # fire
    ("水", "sui"),    # water
    ("木", "moku")),   # tree
    (("金", "kin"),    # gold
    ("土", "do"),     # earth
    ("山", "yama"),   # mountain
    ("川", "kawa"),   # river
    ("人", "hito")),   # person
    (("口", "kuchi"),  # mouth
    ("目", "me"),     # eye
    ("耳", "mimi"),   # ear
    ("手", "te"),     # hand
    ("足", "ashi")),   # foot
    (("力", "chikara"),) # power
]



# --- UI Setup Class ---
class QuizWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Japanese Alphabet Quiz")
        # Default geometry, will be overridden by prefs if present
        self.setGeometry(100, 100, 1200, 700)
        self.layout = QVBoxLayout()

        # For saving/loading preferences
        self.prefs_file = "japanese_alphabet_quiz_prefs.json"

        # Checkbox to hide/show tables and language checkboxes
        self.hide_tables_cb = QCheckBox("Hide alphabet tables and script checkboxes", self)
        self.hide_tables_cb.setChecked(False)
        self.hide_tables_cb.stateChanged.connect(self.toggle_tables_and_checkboxes)
        self.layout.addWidget(self.hide_tables_cb)

        # Number input for times_to_show
        from PyQt5.QtWidgets import QSpinBox, QLabel, QHBoxLayout
        self.times_label = QLabel("Times to show each character:", self)
        self.times_spin = QSpinBox(self)
        self.times_spin.setMinimum(1)
        self.times_spin.setMaximum(20)
        self.times_spin.setValue(1)
        self.times_spin.setFixedWidth(35)  # Only wide enough for 2 digits
        self.times_spin.valueChanged.connect(self.save_checkbox_preferences)
        times_layout = QHBoxLayout()
        times_layout.setSpacing(2)  # Remove spacing between label and spinbox
        times_layout.setAlignment(Qt.AlignLeft)  # Left justify the layout
        times_layout.addWidget(self.times_label)
        times_layout.addWidget(self.times_spin)
        self.layout.addLayout(times_layout)

        # Load preferences after widgets are created
        self._prefs_loaded = False

        # Tables for Hiragana, Katakana, Kanji with row checkboxes
        self.tables_layout = QHBoxLayout()
        self.hiragana_table, self.hiragana_row_checks = self.create_alphabet_table_with_checkboxes(HIRAGANA_ROMAJI, "Hiragana")
        self.katakana_table, self.katakana_row_checks = self.create_alphabet_table_with_checkboxes(KATAKANA_ROMAJI, "Katakana")
        self.kanji_table, self.kanji_row_checks = self.create_alphabet_table_with_checkboxes(KANJI_ROMAJI, "Kanji")
        # Initial width will be set in resizeEvent
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
        self.checkbox_layout.addWidget(self.hiragana_cb)

        self.katakana_cb = QCheckBox("Katakana", self)
        self.katakana_cb.setChecked(True)
        self.checkbox_layout.addWidget(self.katakana_cb)

        self.kanji_cb = QCheckBox("Kanji", self)
        self.kanji_cb.setChecked(True)
        self.checkbox_layout.addWidget(self.kanji_cb)

        self.layout.addLayout(self.checkbox_layout)

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
        self.layout.addWidget(self.input)

        # Bottom message label
        self.bottom_label = QLabel("", self)
        self.bottom_label.setAlignment(Qt.AlignCenter)
        self.bottom_label.setStyleSheet("font-size: 16px; color: red;")
        self.layout.addWidget(self.bottom_label)

        self.setLayout(self.layout)

        # Load preferences (after all widgets are set up)
        self.load_checkbox_preferences()
        self._prefs_loaded = True

    def toggle_tables_and_checkboxes(self, state):
        hide = state == Qt.Checked
        self.hiragana_table.setVisible(not hide)
        self.katakana_table.setVisible(not hide)
        self.kanji_table.setVisible(not hide)
        for cb in [self.hiragana_cb, self.katakana_cb, self.kanji_cb]:
            cb.setVisible(not hide)
        self.tables_layout.setEnabled(not hide)
        self.checkbox_layout.setEnabled(not hide)

    def save_checkbox_preferences(self):
        import json
        prefs = {
            'hiragana_cb': self.hiragana_cb.isChecked(),
            'katakana_cb': self.katakana_cb.isChecked(),
            'kanji_cb': self.kanji_cb.isChecked(),
            'hide_tables_cb': self.hide_tables_cb.isChecked(),
            'hiragana_rows': [cb.isChecked() for cb in self.hiragana_row_checks],
            'katakana_rows': [cb.isChecked() for cb in self.katakana_row_checks],
            'kanji_rows': [cb.isChecked() for cb in self.kanji_row_checks],
            'window_size': [self.width(), self.height()],
            'times_to_show': self.times_spin.value(),
        }
        try:
            with open(self.prefs_file, 'w') as f:
                json.dump(prefs, f)
        except Exception as e:
            print(f"Failed to save preferences: {e}")

    def load_checkbox_preferences(self):
        import json
        import os
        if not os.path.exists(self.prefs_file):
            return
        try:
            with open(self.prefs_file, 'r') as f:
                prefs = json.load(f)
            # Set checkboxes
            self.hiragana_cb.setChecked(prefs.get('hiragana_cb', True))
            self.katakana_cb.setChecked(prefs.get('katakana_cb', True))
            self.kanji_cb.setChecked(prefs.get('kanji_cb', True))
            self.hide_tables_cb.setChecked(prefs.get('hide_tables_cb', False))
            for cb, val in zip(self.hiragana_row_checks, prefs.get('hiragana_rows', [])):
                cb.setChecked(val)
            for cb, val in zip(self.katakana_row_checks, prefs.get('katakana_rows', [])):
                cb.setChecked(val)
            for cb, val in zip(self.kanji_row_checks, prefs.get('kanji_rows', [])):
                cb.setChecked(val)
            # Set times_to_show
            times_val = prefs.get('times_to_show', 1)
            self.times_spin.setValue(times_val if isinstance(times_val, int) and times_val > 0 else 1)
            # Disable tables if their script checkbox is not checked
            self.hiragana_table.setDisabled(not self.hiragana_cb.isChecked())
            self.katakana_table.setDisabled(not self.katakana_cb.isChecked())
            self.kanji_table.setDisabled(not self.kanji_cb.isChecked())
            # Restore window size if present
            if 'window_size' in prefs and isinstance(prefs['window_size'], list) and len(prefs['window_size']) == 2:
                w, h = prefs['window_size']
                self.resize(w, h)
        except Exception as e:
            print(f"Failed to load preferences: {e}")
    def resizeEvent(self, event):
        # Set each table to 1/3 of the window width
        total_width = self.width()
        table_width = max(100, total_width // 3)
        for table in [self.hiragana_table, self.katakana_table, self.kanji_table]:
            #table.setMinimumWidth(table_width)
            table.setMaximumWidth(table_width)
        # Save window size on resize if prefs are loaded
        if getattr(self, '_prefs_loaded', False):
            self.save_checkbox_preferences()
        super().resizeEvent(event)

    def create_alphabet_table_with_checkboxes(self, alphabet, title):
        from PyQt5.QtWidgets import QCheckBox, QWidget, QHBoxLayout
        from PyQt5.QtGui import QFont
        n = len(alphabet)
        max_row_len = max(len(row) for row in alphabet)
        table = QTableWidget()
        table.setColumnCount(max_row_len + 1)  # +1 for checkbox
        table.setRowCount(n)
        table.horizontalHeader().setVisible(False)
        table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        table.verticalHeader().setVisible(False)
        font = QFont()
        font.setPointSize(12)
        row_checks = []

        # Create the table structure
        for row in range(n):
            cb = QCheckBox()
            cb.setChecked(True)
            row_checks.append(cb)
            widget = QWidget()
            layout = QHBoxLayout(widget)
            layout.addWidget(cb)
            layout.setAlignment(cb, Qt.AlignCenter)
            layout.setContentsMargins(0, 0, 0, 0)
            widget.setLayout(layout)
            table.setCellWidget(row, 0, widget)

        # Fill table with char (romaji)
        for rowIdx, rowData in enumerate(alphabet):
            for idx, pair in enumerate(rowData):
                if len(pair) == 2:
                    char, romaji = pair
                    if not char:
                        continue
                    col = idx + 1  # +1 for checkbox
                    item = QTableWidgetItem(f"{char} ({romaji})")
                    item.setFont(font)
                    table.setItem(rowIdx, col, item)

        table.setEditTriggers(QTableWidget.NoEditTriggers)
        table.setSelectionMode(QTableWidget.NoSelection)
        table.setSizeAdjustPolicy(QTableWidget.AdjustToContents)
        table.resizeColumnsToContents()
        table.resizeRowsToContents()
        return table, row_checks

# --- Quiz Logic Class ---
class QuizGame:
    def __init__(self, window: QuizWindow):
        import pyttsx3
        self.window = window
        self.correct_count = 0
        self.retry_count = 0
        self.last_wrong = None
        self.char_weights = {}  # (char, romaji): weight
        self.answered_once = set()  # (char, romaji) that have been answered correctly at least once
        self.char_stats = {}  # (char, romaji): {'shown': int, 'correct': int, 'incorrect': int}
        self.times_to_show = self.window.times_spin.value() if hasattr(self.window, 'times_spin') else 1
        self.round_chars = []
        self.unanswered_chars = []
        self.tts_engine = pyttsx3.init()
        self.tts_engine.setProperty('rate', 150)
        self.connect_signals()
        self.new_quiz()
    def new_quiz(self):
        # Update times_to_show from UI
        self.times_to_show = self.window.times_spin.value() if hasattr(self.window, 'times_spin') else 1
        # Setup the round characters and unanswered characters for a new quiz round
        enabled = self.get_enabled_alphabets()
        all_chars = [item for sublist in enabled for item in sublist]
        self.answered_once.clear()
        self.char_stats.clear()
        self.char_weights.clear()
        self.round_chars = []
        for c in all_chars:
            self.round_chars.extend([c] * self.times_to_show)
        char_counts = {}
        for c in self.answered_once:
            char_counts[c] = char_counts.get(c, 0) + 1
        self.unanswered_chars = []
        for c in self.round_chars:
            if char_counts.get(c, 0) < self.times_to_show:
                self.unanswered_chars.append(c)
                char_counts[c] = char_counts.get(c, 0) + 1
        self.window.char_label.setText("")
        self.window.bottom_label.setText("")
        self.correct_count = 0
        self.retry_count = 0
        self.update_score()
        self.new_question()

    def pronounce_romaji(self, romaji=None):
        # Use gTTS for natural Japanese TTS (requires internet)
        # If called from button, the first arg is not a string
        if not isinstance(romaji, str):
            romaji = getattr(self, 'current_romaji', None)
        kana = getattr(self, 'current_char', None)
        if not romaji and not kana:
            return
        try:
            from gtts import gTTS
            import tempfile, os
            from playsound import playsound
            # Prefer pronouncing kana/kanji if available, else romaji
            text = kana if kana else romaji
            tts = gTTS(text, lang='ja')
            with tempfile.NamedTemporaryFile(delete=False, suffix='.mp3') as fp:
                tts.save(fp.name)
                playsound(fp.name)
            os.remove(fp.name)
        except Exception as e:
            # Fallback to pyttsx3 if gTTS or playsound is not available or fails
            try:
                if romaji:
                    self.tts_engine.say(romaji)
                    self.tts_engine.runAndWait()
            except Exception as e2:
                print(f"TTS failed: {e2}")

    def connect_signals(self):
        # Connect the times_spin valueChanged signal to new_quiz        
        self.window.times_spin.valueChanged.connect(self.new_quiz)
        # Save preferences on any relevant checkbox change
        self.window.hiragana_cb.stateChanged.connect(lambda state: (self.handle_script_checkbox('hiragana', state), self.window.save_checkbox_preferences()) if self.window._prefs_loaded else self.handle_script_checkbox('hiragana', state))
        self.window.katakana_cb.stateChanged.connect(lambda state: (self.handle_script_checkbox('katakana', state), self.window.save_checkbox_preferences()) if self.window._prefs_loaded else self.handle_script_checkbox('katakana', state))
        self.window.kanji_cb.stateChanged.connect(lambda state: (self.handle_script_checkbox('kanji', state), self.window.save_checkbox_preferences()) if self.window._prefs_loaded else self.handle_script_checkbox('kanji', state))
        self.window.hide_tables_cb.stateChanged.connect(lambda state: self.window.save_checkbox_preferences() if self.window._prefs_loaded else None)
        self.window.input.returnPressed.connect(self.check_answer)
        # Connect row checkbox signals for all scripts
        for cb in self.window.hiragana_row_checks:
            cb.stateChanged.connect(lambda state, script='hiragana', cb=cb: (self.handle_row_checkbox(script, cb, state), self.window.save_checkbox_preferences()) if self.window._prefs_loaded else self.handle_row_checkbox(script, cb, state))
        for cb in self.window.katakana_row_checks:
            cb.stateChanged.connect(lambda state, script='katakana', cb=cb: (self.handle_row_checkbox(script, cb, state), self.window.save_checkbox_preferences()) if self.window._prefs_loaded else self.handle_row_checkbox(script, cb, state))
        for cb in self.window.kanji_row_checks:
            cb.stateChanged.connect(lambda state, script='kanji', cb=cb: (self.handle_row_checkbox(script, cb, state), self.window.save_checkbox_preferences()) if self.window._prefs_loaded else self.handle_row_checkbox(script, cb, state))

    def highlight_char_in_tables(self, char, romaji, color):
        def highlight_in_table(table, alphabet, char, romaji, color):
            for rowIdx, rowData in enumerate(alphabet):
                for col, pair in enumerate(rowData):
                    if len(pair) == 2:
                        c, r = pair
                        if c == char and r == romaji:
                            item = table.item(rowIdx, col + 1)
                            if item:
                                if color == 'green':
                                    item.setForeground(Qt.green)
                                elif color == 'red':
                                    item.setForeground(Qt.red)
                                else:
                                    item.setForeground(Qt.black)
        highlight_in_table(self.window.hiragana_table, HIRAGANA_ROMAJI, char, romaji, color)
        highlight_in_table(self.window.katakana_table, KATAKANA_ROMAJI, char, romaji, color)
        highlight_in_table(self.window.kanji_table, KANJI_ROMAJI, char, romaji, color)

    def reset_all_table_highlights(self):
        def reset_table(table, alphabet):
            for rowIdx, rowData in enumerate(alphabet):
                for col, pair in enumerate(rowData):
                    if len(pair) == 2:
                        item = table.item(rowIdx, col + 1)
                        if item:
                            if table.isEnabled():
                                item.setForeground(Qt.white)
        reset_table(self.window.hiragana_table, HIRAGANA_ROMAJI)
        reset_table(self.window.katakana_table, KATAKANA_ROMAJI)
        reset_table(self.window.kanji_table, KANJI_ROMAJI)

    def handle_script_checkbox(self, script, state):
        if script == 'hiragana':
            table = self.window.hiragana_table
        elif script == 'katakana':
            table = self.window.katakana_table
        elif script == 'kanji':
            table = self.window.kanji_table
        else:
            return
        block = True if state == 0 else False
        table.setDisabled(block)
        self.new_quiz()

    def new_question(self, exclude_current=False):
        self.window.bottom_label.setText("")
        enabled = self.get_enabled_alphabets()
        if not enabled:
            self.window.char_label.setText("")
            QMessageBox.warning(self.window, "No Script Selected", "Please select at least one script to practice.")
            self.window.input.setDisabled(True)
            return
        else:
            self.window.input.setDisabled(False)
        if not self.unanswered_chars:
            # All done for this round
            all_chars = [item for sublist in enabled for item in sublist]
            self.show_summary_and_reset(all_chars)
            return
        current_tuple = getattr(self, 'current_char', None), getattr(self, 'current_romaji', None)
        available = self.unanswered_chars
        if exclude_current and len(available) > 1:
            available = [c for c in available if c != current_tuple]
        chosen = random.choice(available)
        self.current_char, self.current_romaji = chosen
        self.last_wrong = None
        char_tuple = (self.current_char, self.current_romaji)
        if char_tuple not in self.char_stats:
            self.char_stats[char_tuple] = {'shown': 0, 'correct': 0, 'incorrect': 0}
        self.char_stats[char_tuple]['shown'] += 1
        self.window.char_label.setText(self.current_char)
        self.window.input.clear()

    # ...existing code...
    def handle_row_checkbox(self, script, cb, state):
        # Get the correct row_checks list
        if script == 'hiragana':
            row_checks = self.window.hiragana_row_checks
        elif script == 'katakana':
            row_checks = self.window.katakana_row_checks
        elif script == 'kanji':
            row_checks = self.window.kanji_row_checks
        else:
            return
        # Prevent all from being unchecked
        if state == 0:
            if sum(1 for c in row_checks if c.isChecked()) == 0:
                cb.blockSignals(True)
                cb.setChecked(True)
                cb.blockSignals(False)
                return
        self.new_quiz()

    def update_score(self):
        self.window.score_label.setText(f"Correct: {self.correct_count} | Retries: {self.retry_count}")

    def get_enabled_alphabets(self):
        enabled = []
        def get_checked_rows(row_checks, alphabet):
            chars = []
            for row, cb in enumerate(row_checks):
                if cb.isChecked():
                    for pair in alphabet[row]:
                        if not pair[0]:  # Skip empty pairs
                            continue
                        if len(pair) == 2:
                            chars.append(pair)
            return chars
        if self.window.hiragana_cb.isChecked():
            enabled.append(get_checked_rows(self.window.hiragana_row_checks, HIRAGANA_ROMAJI))
        if self.window.katakana_cb.isChecked():
            enabled.append(get_checked_rows(self.window.katakana_row_checks, KATAKANA_ROMAJI))
        if self.window.kanji_cb.isChecked():
            enabled.append(get_checked_rows(self.window.kanji_row_checks, KANJI_ROMAJI))
        all_chars = [item for sublist in enabled for item in sublist]
        for char in all_chars:
            if char not in self.char_weights:
                self.char_weights[char] = 1.0
        to_remove = [k for k in self.char_weights if k not in all_chars]
        for k in to_remove:
            del self.char_weights[k]
            self.answered_once.discard(k)
        return enabled

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
        QMessageBox.information(self.window, "Quiz Summary", msg)
        # Reset for next round
        self.reset_all_table_highlights()
        self.new_quiz()  # Start a new quiz round
        self.update_score()

    def check_answer(self):
        from PyQt5.QtCore import QTimer
        user_input = self.window.input.text().strip().lower()
        self.window.bottom_label.setText("")
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
            #remove from unanswered_chars
            self.unanswered_chars.remove(char_tuple)
            self.update_score()
            # This will prevent the next question from being the same character
            if len(self.unanswered_chars) > 1:
                self.new_question(exclude_current=True)
            else:
                self.new_question()
        else:
            self.highlight_char_in_tables(self.current_char, self.current_romaji, 'red')
            self.retry_count += 1
            self.char_stats[char_tuple]['incorrect'] += 1
            self.update_score()
            self.window.bottom_label.setText(f"Incorrect! {self.current_char} = {self.current_romaji}")
            self.flash_char_label_red_then_next()

    def flash_char_label_red_then_next(self):
        from PyQt5.QtCore import QTimer
        flashes = ["color: red; font-size: 64px;", "color: black; font-size: 64px;"] * 3
        def do_flash():
            if flashes:
                self.window.char_label.setStyleSheet(flashes.pop(0))
                QTimer.singleShot(170, do_flash)
            else:
                self.window.char_label.setStyleSheet("font-size: 64px;")
                QTimer.singleShot(50, lambda: self.new_question(exclude_current=True))  # Ensure new_question runs after the event loop processes
        do_flash()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = QuizWindow()
    game = QuizGame(window)
    window.show()
    sys.exit(app.exec_())
