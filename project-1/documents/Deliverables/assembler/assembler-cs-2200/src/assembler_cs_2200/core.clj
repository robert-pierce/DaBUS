(ns assembler-cs-2200.core
  (:require [clojure.java.io :as io]
            [clojure.string :as str])
  (:gen-class))

(def inst-type
  {"add" "R-Type",
   "addi" "I-Type",
   "mul"  "R-Type",
   "sll"  "I-Type",
   "srl"  "I-Type",
   "and"  "R-Type",
   "or"   "R-Type",
   "lw"   "LD-Type",
   "sw"   "LD-Type",
   "j"    "J-Type",
   "jr"   "JR-Type",
   "jal"  "J-Type",
   "beq"  "I-Type",
   "halt" "O-Type"})

(def opcodes
  {"add"  "000001",
   "addi" "000010",
   "mul"  "000011",
   "sll"  "000100",
   "srl"  "000101",
   "and"  "000110",
   "or"   "000111",
   "lw"   "001000",
   "sw"   "001001",
   "j"    "001010",
   "jr"   "001011",
   "jal"  "001100",
   "beq"  "001101",
   "halt" "001110"})

(def registers
  {"$0"    "00000",
   "$zero" "00000",
   "$at"   "00001",
   "$1"    "00001",
   "$v0"   "00010",
   "$2"    "00010",
   "$v1"   "00011",
   "$3"    "00011",
   "$a0"   "00100",
   "$4"    "00100",
   "$a1"   "00101",
   "$5"    "00101",
   "$a2"   "00110",
   "$6"    "00110",
   "$a3"   "00111",
   "$7"    "00111",
   "$t0"   "01000",
   "$8"    "01000",
   "$t1"   "01001",
   "$9"    "01001",
   "$t2"   "01010",
   "$10"   "01010",
   "$t3"   "01011",
   "$11"   "01011",
   "$t4"   "01100",
   "$12"   "01100",
   "$t5"   "01101",
   "$13"   "01101",
   "$t6"   "01110",
   "$14"   "01110",
   "$t7"   "01111",
   "$15"   "01111",
   "$s0"   "10000",
   "$16"   "10000",
   "$s1"   "10001",
   "$17"   "10001",
   "$s2"   "10010",
   "$18"   "10010",
   "$s3"   "10011",
   "$19"   "10011",
   "$s4"   "10100",
   "$20"   "10100",
   "$s5"   "10101",
   "$21"   "10101",
   "$s6"   "10110",
   "$22"   "10110",
   "$s7"   "10111",
   "$23"   "10111",
   "$t8"   "11000",
   "$24"   "11000",
   "$t9"   "11001",
   "$25"   "11001",
   "$k0"   "11010",
   "$26"   "11010",
   "$k1"   "11011",
   "$27"   "11011",
   "$gp"   "11100",
   "$28"   "11100",
   "$sp"   "11101",
   "$29"   "11101",
   "$fp"   "11110",
   "$30"   "11110",
   "$ra"   "11111",
   "$31"   "11111"})


(defn bin-str-to-hex
  "parses a binary string into a hex string"
  [bin-str-in]
  (Integer/toString 
   (Integer/parseInt bin-str-in 2) 16))

(defn int-to-hex
  "parses an integer into a 16 bit (4 digit) hex string"
  [int-in]
  (if (nil? int-in)
    (str "0000")
    (let [parsed-int (Integer/parseInt int-in)] 
      (if (< parsed-int 0)
        (apply str (drop 4 (format "%04x" parsed-int)))
        (format "%04x" parsed-int)))))

(defn pad-binary-zeros
  [bin-in desired-length]
  (str (reduce str (repeat (- desired-length (count bin-in)) "0")) bin-in))

(defn hex-target-to-binary
  "parses a hex string into a binary string of desired length"
  [hex-in desired-length]
  (pad-binary-zeros (Integer/toBinaryString (read-string hex-in)) 
                    desired-length))

(defn parse-load-arg
  "Parse the special arg format for load instruction:  offset($reg)
  Returns a vec [Offset register] where both are strings and regsiter 
  is in 5 bit binary"
  [arg-str-in]
  (let [arg-vec (str/split arg-str-in #"[()]")]
    (assoc arg-vec 1 (get registers (last arg-vec)))))

(defn init-program
  "This function writes instructions or text to the out-file that must
  be included in order for the program to function properly."
  [file-out]
  (do 
    ; add "v2.0 raw" b/c it is required for logisim to load an image into ram
    (spit file-out "v2.0 raw\n" :append true)
    ; set $sp to address 10,000, for this project this will be the top of the stack
    ; addi $sp $zero 10,000
    (spit file-out "081d2710\n" :append true)))

(defn process-r-type
  "Processes an R-Type instruction"
  [instruction-map file-out]
  (let [opcode (get opcodes (get instruction-map :op))
        Rd (get registers (get instruction-map :arg1))
        Rb (get registers (get instruction-map :arg2))
        Rc (get registers(get instruction-map :arg3))]
    (do
      (println (str  (get instruction-map :op) " " 
                     (get instruction-map :arg1) " " 
                     (get instruction-map :arg2) " " 
                     (get instruction-map :arg3)))

     (println (str "Opcode: " opcode 
                   " Rb " Rb 
                   " Rc " Rc 
                   " Rd " Rd))

     (println (bin-str-to-hex (str opcode Rb Rc Rd "00000000000")))

     (spit file-out 
           (str (bin-str-to-hex (str opcode Rb Rc Rd "00000000000")) "\n")
           :append true))))

(defn process-i-type
  "Processes an I-Type instructin"
  [instruction-map file-out]
  (let [opcode (get opcodes (get instruction-map :op))
        Rd (get registers (get instruction-map :arg1))
        Rb (get registers (get instruction-map :arg2))
        Imm (get instruction-map :arg3)]
    
    (do
      (println 
       (str  (get instruction-map :op) " " 
             (get instruction-map :arg1) " " 
             (get instruction-map :arg2) " " 
             (get instruction-map :arg3)))

      (println (str "Opcode: " opcode
                    " Rb: " Rb 
                    " Rd: " Rd
                    " Imm: " Imm))

      (println (str (bin-str-to-hex 
                     (str opcode Rb Rd))
                    (int-to-hex Imm)))

      (spit file-out
            (str (bin-str-to-hex 
                  (str opcode Rb Rd))
                 (int-to-hex Imm) "\n")
            :append true))))

(defn process-j-type
  "Processes an J-Type instruction"
  [instruction-map file-out]
  (let [opcode (get opcodes (get instruction-map :op))
        Target (get instruction-map :arg1)]

    (do 
      (println 
       (str (get instruction-map :op) " "
            (get instruction-map :arg1)))

      (println (str "Opcode: " opcode
                    " Target " Target))

      (println (str (bin-str-to-hex (str opcode 
                                         (hex-target-to-binary Target 26)))))

      (spit file-out
            (str (bin-str-to-hex (str opcode 
                                      (hex-target-to-binary Target 26))) "\n")
            :append true))))

(defn process-o-type
  "Process an O-Type instruction"
  [instruction-map file-out]
  (let [opcode (get opcodes (get instruction-map :op))]
    (do
      (println (str (get instruction-map :op)))
      
      (println (str "Opcode: " opcode))

      (println (str (bin-str-to-hex (str opcode "00000000000000000000000000"))))

      (spit file-out
            (str (bin-str-to-hex (str opcode "00000000000000000000000000")) "\n")
            :append true))))

(defn process-jr-type
 "Process the special case of the jal instruction"
 [instruction-map file-out]
 (let [opcode (get opcodes (get instruction-map :op))
       Rb (get registers (get instruction-map :arg1))]
   (do
     (println 
       (str  (get instruction-map :op) " " 
             (get instruction-map :arg1)))

     (println (str "Opcode: " opcode 
                   " Rb " Rb))

     (println (bin-str-to-hex 
                (str opcode Rb "00000" "0000000000000000")))

     (spit file-out 
           (str (bin-str-to-hex (str opcode Rb "00000" "0000000000000000")) "\n")
           :append true))))

(defn process-load-type
  "Process the special case of load types with syntax off($reg)"
  [instruction-map file-out]
  (let [opcode (get opcodes (get instruction-map :op))
        Rd (get registers (get instruction-map :arg1))
        [Offset Rb] (parse-load-arg (get instruction-map :arg2))]
    
    (do
      (println 
       (str  (get instruction-map :op) " " 
             (get instruction-map :arg1) " "
             (get instruction-map :arg2)))

      (println (str "Opcode: " opcode 
                   " Rb " Rb
                   " Rd " Rd
                   " Offset " Offset))

      (println (str (bin-str-to-hex 
                     (str opcode Rb Rd))
                    (int-to-hex Offset)))
      
      (spit file-out
            (str (bin-str-to-hex 
                  (str opcode Rb Rd))
                 (int-to-hex Offset) "\n")
            :append true))))

(defn -main
  "This is an assembler for cs-2200 project) 1"
  [file-in file-out]
  (with-open [rdr (io/reader file-in)]
    (do
      (init-program file-out)

      (doseq [line (line-seq rdr)]
        (let [inst-map 
              (zipmap [:op :arg1 :arg2 :arg3] 
                      (map str/trim (str/split line #"[,;]")))]
          (if-let [instruction-type (get inst-type (get inst-map :op))]
            (do
              (if (= instruction-type "R-Type")
                (process-r-type inst-map file-out)
                (if (= instruction-type "I-Type")
                  (process-i-type inst-map file-out)
                  (if (= instruction-type "J-Type")
                    (process-j-type inst-map file-out)
                    (if (= instruction-type "JR-Type")
                      (process-jr-type inst-map file-out)
                      (if (= instruction-type "LD-Type")
                        (process-load-type inst-map file-out)
                        (if (= instruction-type "O-Type")
                          (process-o-type inst-map file-out))))))))))))))
