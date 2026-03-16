#!/usr/bin/env bash
# =============================================================================
# setup_riscv_lab.sh
# Sets up the riscv-assembler-lab project structure on WSL or macOS.
# All source files are created as empty stubs, ready for vibe coding.
# =============================================================================

set -euo pipefail

# ── Colours ──────────────────────────────────────────────────────────────────
RED='\033[0;31m'
GREEN='\033[0;32m'
CYAN='\033[0;36m'
YELLOW='\033[1;33m'
BOLD='\033[1m'
RESET='\033[0m'

# ── Helpers ───────────────────────────────────────────────────────────────────
info()    { echo -e "${CYAN}[INFO]${RESET}  $*"; }
success() { echo -e "${GREEN}[OK]${RESET}    $*"; }
warn()    { echo -e "${YELLOW}[WARN]${RESET}  $*"; }
error()   { echo -e "${RED}[ERR]${RESET}   $*" >&2; exit 1; }

# ── Platform detection ────────────────────────────────────────────────────────
detect_platform() {
  case "$(uname -s)" in
    Linux*)
      if grep -qi microsoft /proc/version 2>/dev/null; then
        echo "WSL"
      else
        echo "Linux"
      fi
      ;;
    Darwin*) echo "macOS" ;;
    *)       echo "Unknown" ;;
  esac
}

PLATFORM=$(detect_platform)
info "Detected platform: ${BOLD}${PLATFORM}${RESET}"

# ── Root directory ────────────────────────────────────────────────────────────
ROOT="riscv-assembler-lab"

if [[ -d "$ROOT" ]]; then
  warn "Directory '${ROOT}' already exists."
  read -rp "$(echo -e "${YELLOW}Overwrite / re-create missing files? [y/N]: ${RESET}")" yn
  [[ "${yn,,}" == "y" ]] || { info "Aborted. Nothing was changed."; exit 0; }
fi

# ── Directory layout ──────────────────────────────────────────────────────────
DIRS=(
  "${ROOT}/src"
  "${ROOT}/tests"
)

# ── Files to create (relative to ROOT) ───────────────────────────────────────
FILES=(
  # Build / docs
  "Makefile"
  "README.md"
  # Sources
  "src/registers.c"
  "src/registers.h"
  "src/parser.c"
  "src/parser.h"
  "src/instr_table.c"
  "src/instr_table.h"
  "src/symbols.c"
  "src/symbols.h"
  "src/encode.c"
  "src/encode.h"
  "src/assembler.c"
  "src/assembler.h"
  "src/main.c"
  # Tests
  "tests/test_registers.c"
  "tests/test_parser.c"
  "tests/test_instr_table.c"
  "tests/test_symbols.c"
  "tests/test_encode.c"
  "tests/test_assembler.c"
)

# ── Create directories ────────────────────────────────────────────────────────
echo ""
info "Creating directories…"
for d in "${DIRS[@]}"; do
  mkdir -p "$d"
  success "mkdir  $d"
done

# ── Create files ──────────────────────────────────────────────────────────────
echo ""
info "Creating stub files…"
for f in "${FILES[@]}"; do
  target="${ROOT}/${f}"
  if [[ ! -f "$target" ]]; then
    touch "$target"
    success "touch  $target"
  else
    warn   "exists $target  (skipped)"
  fi
done

# ── Pretty tree ───────────────────────────────────────────────────────────────
echo ""
info "Project layout:"
echo ""

print_tree() {
  local dir="$1"
  local prefix="$2"

  # Collect entries, dirs first then files, sorted
  local entries=()
  while IFS= read -r entry; do
    entries+=("$entry")
  done < <(ls -1 "$dir" 2>/dev/null | sort)

  local count=${#entries[@]}
  local i=0
  for entry in "${entries[@]}"; do
    (( i++ )) || true
    local path="${dir}/${entry}"
    local connector="├──"
    local child_prefix="${prefix}│   "
    if [[ $i -eq $count ]]; then
      connector="└──"
      child_prefix="${prefix}    "
    fi
    if [[ -d "$path" ]]; then
      echo -e "${prefix}${connector} ${BOLD}${CYAN}${entry}/${RESET}"
      print_tree "$path" "${child_prefix}"
    else
      echo "${prefix}${connector} ${entry}"
    fi
  done
}

echo -e "${BOLD}${CYAN}${ROOT}/${RESET}"
print_tree "$ROOT" ""

# ── Done ──────────────────────────────────────────────────────────────────────
echo ""
success "Setup complete! ${BOLD}${#FILES[@]}${RESET} files ready inside ${BOLD}./${ROOT}/${RESET}"
echo ""
echo -e "  ${CYAN}cd ${ROOT} && ls${RESET}   — explore the layout"
echo -e "  ${CYAN}make${RESET}              — build once Makefile is filled in"
echo ""
