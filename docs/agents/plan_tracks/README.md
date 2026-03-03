# Plan Tracks (v5/v6)

This directory stores future-version planning tracks and long-form roadmap artifacts.

## Canonical rule
- `docs/agents/Plans.md` remains the only execution ledger.
- Any executable plan must be appended to `Plans.md` first.
- Files under `plan_tracks/` are planning companions (roadmaps, breakdowns, decisions, notes).

## Structure
- `v5/` and `v6/` track version-level planning.
- `index.md` defines scope and objectives.
- `roadmap.md` lists milestones.
- `milestones/` stores one file per milestone package.
- `decisions.md` stores decision records specific to that track.

## Linking requirement
When a `Plans.md` entry belongs to a track, add `refs` to one or more files in that track.
