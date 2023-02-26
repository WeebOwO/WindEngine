#pragma once

#define DELETE_COPY(Class) Class(const Class&) = delete; \
Class& operator=(const Class&) = delete;

#define DELETE_MOVE(Class) Class(Class&&) = delete;\
Class& operator=(Class&&) = delete;
