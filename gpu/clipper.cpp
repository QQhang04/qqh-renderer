#include "clipper.h"

bool Clipper::cullFace(
	const uint32_t& frontFace,
	const uint32_t& cullFace,
	const VsOutput& v0,
	const VsOutput& v1,
	const VsOutput& v2) {

	math::vec3f edge1 = v1.mPosition - v0.mPosition;
	math::vec3f edge2 = v2.mPosition - v0.mPosition;

	math::vec3f normal = math::cross(edge1, edge2);

	if (cullFace == BACK_FACE) {
		if (frontFace == FRONT_FACE_CCW) {
			return normal.z > 0;
		}
		else {
			return normal.z < 0;
		}
	}
	else {
		if (frontFace == FRONT_FACE_CCW) {
			return normal.z < 0;
		}
		else {
			return normal.z > 0;
		}
	}
}

void Clipper::doClipSpace(const uint32_t& drawMode, const std::vector<VsOutput>& primitives, std::vector<VsOutput>& outputs) {
	outputs.clear();

	if (drawMode == DRAW_TRIANGLES) {
		std::vector<VsOutput> primitive;
		std::vector<VsOutput> results;
		
		for (int i = 0; i < primitives.size(); i += 3) {
			primitive.clear();
			results.clear();

			primitive.assign(primitives.begin() + i, primitives.begin() + i + 3);

			Clipper::sutherlandHodgman(drawMode, primitive, results);

			if (results.empty()) continue;

			for (int j = 0; j < results.size() - 2; j++) {
				outputs.push_back(results[0]);
				outputs.push_back(results[j + 1]);
				outputs.push_back(results[j + 2]);
			}
		}
	}
	else if (drawMode == DRAW_LINES) {
		std::vector<VsOutput> primitive;
		std::vector<VsOutput> results;

		for (uint32_t i = 0; i < primitives.size(); i += 2) {
			primitive.clear();
			results.clear();
			auto start = primitives.begin() + i;
			auto end = primitives.begin() + i + 2;
			primitive.assign(start, end);
			Clipper::sutherlandHodgman(drawMode, primitive, results);

			outputs.push_back(results[0]);
			outputs.push_back(results[1]);
		}
	}
}

void Clipper::sutherlandHodgman(const uint32_t& drawMode, const std::vector<VsOutput>& primitive, std::vector<VsOutput>& outputs) {
	assert(outputs.size() == 0);
	outputs = primitive;
	std::vector<VsOutput> inputs;
	std::vector<math::vec4f> clipPlanes = {
		// judge w > 0
		math::vec4f(0.0f, 0.0f, 0.0f, 1.0f),
		// near
		math::vec4f(0.0f, 0.0f, 1.0f, 1.0f),
		// far
		math::vec4f(0.0f, 0.0f, -1.0f, 1.0f),
		// left
		math::vec4f(1.0f, 0.0f, 0.0f, 1.0f),
		// right
		math::vec4f(-1.0f, 0.0f, 0.0f, 1.0f),
		// top
		math::vec4f(0.0f, -1.0f, 0.0f, 1.0f),
		// bottom
		math::vec4f(0.0f, 1.0f, 0.0f, 1.0f)
	};

	for (int i = 0; i < clipPlanes.size(); i++) {
		inputs = outputs;
		outputs.clear();

		for (int p = 0; p < inputs.size(); p++) {
			auto current = inputs[p];
			auto last = inputs[(p - 1 + inputs.size()) % inputs.size()];

			if (inside(current.mPosition, clipPlanes[i])) {
				//��������������أ���������������һ�����㣬��׼��ͷ���㽻��
				if (drawMode != DRAW_LINES || p != inputs.size() - 1) {
					if (!inside(last.mPosition, clipPlanes[i])) {
						auto intersectPoint = intersect(last, current, clipPlanes[i]);
						outputs.push_back(intersectPoint);
					}
				}
				outputs.push_back(current);
			}
			else {
				if (drawMode != DRAW_LINES || p != inputs.size() - 1) {
					if (inside(last.mPosition, clipPlanes[i])) {
						auto intersectPoint = intersect(last, current, clipPlanes[i]);
						outputs.push_back(intersectPoint);
					}
				}
			}
		}
	}
}

bool Clipper::inside(const math::vec4f& point, const math::vec4f& plane) {
	return math::dot(plane, point) > 0;
}

// ����ֻ�ܲ�ֵƽ������ĵ�
VsOutput Clipper::intersect(const VsOutput& last, const VsOutput& current, const math::vec4f& plane) {
	VsOutput output;

	auto distanceLast = math::dot(last.mPosition, plane);
	auto distanceCur = math::dot(current.mPosition, plane);

	float weight = distanceLast / (distanceLast - distanceCur);
	output.mPosition = math::lerp(last.mPosition, current.mPosition, weight);
	output.mColor = math::lerp(last.mColor, current.mColor, weight);
	output.mUV = math::lerp(last.mUV, current.mUV, weight);
	output.mNormal = math::lerp(last.mNormal, current.mNormal, weight);

	return output;
}