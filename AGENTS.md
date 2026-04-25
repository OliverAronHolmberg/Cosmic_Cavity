AGENTS.md Configuration

## File Access Control

- Excludes the following paths from AI tool reads:
- resources/ (image assets used by the game)
- opencode.ignore (explicit ignore list for OpenCode/AI tooling)
- DEX/Temp and other build artifacts if present during debugging

Note: The OpenCode AI tooling should respect opencode.ignore patterns placed at the repo root.
