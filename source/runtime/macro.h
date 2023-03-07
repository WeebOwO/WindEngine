#pragma once

#define PERMIT_COPY(class) class(const class&) = delete; \
class& operator=(const class&) = delete;

#define PERMIT_MOVE(class) class(class&&) = delete; \
class& operator=(class&&) = delete;