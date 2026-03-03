# v7 Decisions

Append track-specific architectural decisions here; mirror execution-impact decisions to `docs/agents/memory.log`.

- 2026-03-03: v7.1 runtime applies graphics state in draw hooks using deterministic precedence (`action override > room > scene > vanilla`) with begin/end env capture+restore safety.
