(define (divide_sort arr) (if (null? arr) '() (let ((first (car arr)) (rest (cdr arr))) (let ( (arr_left (filter (lambda (x) (>= first x)) rest)) (arr_right (filter (lambda (x) (< first x)) rest)) ) (append (divide_sort arr_left) (list first) (divide_sort arr_right)) ) ) ) )
(define x (list 12 71 2 15 29 82 87 8 18 66 81 25 63 97 40 3 93 58 53 31 47))
(print (divide_sort x))