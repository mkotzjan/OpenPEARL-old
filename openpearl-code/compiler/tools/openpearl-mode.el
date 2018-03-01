;;; openpearl-mode.el --- Major mode for editing OpenPEARL source code files.

;; Copyright (c) 2016, Marcel Schaible

;; Author: Marcel Schaible ( marcel (at) fernuni-hagen.de )
;; Version: 1.0.0
;; Created: 20 Jan 2016
;; Keywords: languages
;; Homepage: 

;; This file is not part of GNU Emacs.

;;; License:

;; You can redistribute this program and/or modify it under the terms
;; of the GNU General Public License version 3.

;;; Commentary:

;; short description here

;; full doc on how to use here

;;; Code:

;; define several category of keywords
(setq oprl-keywords '( "A" "ACTIVATE" "ALL" "ALPHIC" "ALT"
		       "BASIC" "BEGIN" "BY"
		       "CASE" "CLOSE" "CONTINUE" "CREATED" "CYCLIC"
		       "DCL" "DECLARE" "DIM" "DIRECT" "DISABLE" "DURING"
		       "ENABLE" "END" "EXIT"
		       "FIN" "FOR" "FORWARD" "FROM"
		       "GLOBAL" "GOTO"
		       "HRS"
		       "IF" "INDUCE" "INIT" "INTERRUPT" "INV"
		       "MAIN" "MIN" "MODULE" "MODEND"
		       "OPEN" "ON" "OUT" 
		       "PRESET" "PRIO" "PROBLEM" "PROC" "PROCEDURE" "PUT"
		       "RELEASE" "REPEAT" "REQUEST" "RST"
		       "SEC" "SEND" "SIGNAL" "SKIP" "SPC" "SPECIFY""SYSTEM"
		       "TAKE" "TASK" "TERMINATE" "THEN" "TO" "TRIGGER" "TRY"
		       "WHEN" "WHILE"
		       ))

(setq oprl-types '("BIT" "CHAR" "CHARACTER" "CLOCK" "DATE" "DATION" "DUR" "DURATION" "FIXED" "FLOAT" "SEMA" "TIME"))

(setq oprl-constants '())

(setq oprl-events '())

(setq oprl-functions '( "ABS"
			"FIT"
			"REM"
			"SIGN" "SIZEOF"
		        "TOFIXED" "TOFLOAT"
		      ))

;; generate regex string for each category of keywords
(setq oprl-keywords-regexp (regexp-opt oprl-keywords 'words))
(setq oprl-type-regexp (regexp-opt oprl-types 'words))
(setq oprl-constant-regexp (regexp-opt oprl-constants 'words))
(setq oprl-event-regexp (regexp-opt oprl-events 'words))
(setq oprl-functions-regexp (regexp-opt oprl-functions 'words))

;; create the list for font-lock.
;; each category of keyword is given a particular face
(setq oprl-font-lock-keywords
      `(
        (,oprl-type-regexp . font-lock-type-face)
        (,oprl-constant-regexp . font-lock-constant-face)
        (,oprl-event-regexp . font-lock-builtin-face)
        (,oprl-functions-regexp . font-lock-function-name-face)
        (,oprl-keywords-regexp . font-lock-keyword-face)
        ;; note: order above matters, because once colored, that part won't change.
        ;; in general, longer words first
        ))

;;;###autoload
(define-derived-mode oprl-mode fundamental-mode
  "OpenPEARL mode"
  "Major mode for editing OpenPEARL"

  ;; code for syntax highlighting
  (setq font-lock-defaults '((oprl-font-lock-keywords))))

;; clear memory. no longer needed
(setq oprl-keywords nil)
(setq oprl-types nil)
(setq oprl-constants nil)
(setq oprl-events nil)
(setq oprl-functions nil)

;; clear memory. no longer needed
(setq oprl-keywords-regexp nil)
(setq oprl-types-regexp nil)
(setq oprl-constants-regexp nil)
(setq oprl-events-regexp nil)
(setq oprl-functions-regexp nil)

;; add the mode to the `features' list
(provide 'oprl-mode)

;; Local Variables:
;; coding: utf-8
;; End:

;;; openpearl-mode.el ends here
