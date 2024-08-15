// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "shader_recompiler/ir/program.h"

namespace Shader::Optimization {

void Visit(Info& info, IR::Block::InstructionList& list, IR::Inst& inst) {
    if (inst.GetOpcode() == IR::Opcode::EmitVertex) {
        IR::Program* copy_program = info.gs_copy_shader;
        ASSERT(copy_program->info.stage == Stage::Vertex);

        // Insert for each instruction on the copy shader
        auto it = list.iterator_to(inst);
        for (IR::Block* copy_block : copy_program->post_order_blocks) {
            for (IR::Inst& copy_inst : copy_block->Instructions()) {
                it = list.insert(it, copy_inst);
            }
        }
    }
}

void MergeCopyShaderPass(IR::Program& program) {
    if (program.info.stage != Shader::Stage::Geometry) {
        return;
    }

    Info& info{program.info};
    for (IR::Block* const block : program.post_order_blocks) {
        IR::Block::InstructionList& list{block->Instructions()};
        for (IR::Inst& inst : list) {
            Visit(info, list, inst);
        }
    }
}

} // namespace Shader::Optimization