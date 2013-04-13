#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"
#include "geomutil.h"

VertexDataList::VertexDataList()
{
	this->num_items = 0;
	this->cur = NULL;
	this->first = NULL;
	this->last = NULL;
}

VertexDataList::~VertexDataList()
{
	vdata_list_item *cur_item = this->first;
    while (cur_item) {
        ninfluence *cur_n;
		vdata *cur_v = cur_item->vd;
        vdata_list_item *next_item = cur_item->next;

        // Free skinning data if any
        if (cur_v->num_bindings) {
            free(cur_v->weights);
            free(cur_v->joints);
        }

        // Free normal influence list
        cur_n = cur_v->first_normal_influence;
        while (cur_n) {
            ninfluence *next_n = cur_n->next;
            free(cur_n);
            cur_n = next_n;
        }

        free(cur_v);
        cur_item = next_item;
    }

	this->clear();
}

void
VertexDataList::append_vdata(vdata *vd)
{
	vdata_list_item *item = (vdata_list_item*)malloc(sizeof(vdata_list_item));
	item->vd = vd;

	if (!this->first) {
		this->first = item;
	}
	else {
		this->last->next = item;
	}

	this->last = item;
	this->last->next = NULL;
	this->num_items++;
}

void
VertexDataList::clear()
{
	this->num_items = 0;
	this->first = NULL;
	this->last = NULL;
}

int
VertexDataList::get_num_items()
{
	return this->num_items;
}

void
VertexDataList::iter_reset()
{
	this->cur = this->first;
}

vdata *
VertexDataList::iter_next()
{
	if (this->cur) {
		vdata *vd = this->cur->vd;
		this->cur = this->cur->next;
		return vd;
	}
	else {
		return NULL;
	}
}

AWDGeomUtil::AWDGeomUtil()
{
	this->expanded = new VertexDataList();
	this->collapsed = new VertexDataList();
	this->per_idx_lists = NULL;
	this->max_orig_idx = 0;
    this->normal_threshold = 0;
    this->joints_per_vertex = 0;
    this->include_uv = true;
    this->include_normals = true;
}

AWDGeomUtil::~AWDGeomUtil()
{
	int i;
	for (i=0; i<num_idx_lists; i++) {
		// Empty list to not delete items, which are
		// duplicated in the expanded list.
		per_idx_lists[i]->clear();
		delete per_idx_lists[i];
	}

	free(per_idx_lists);

	// Empty list to not delete items, which are
	// duplicated in the expanded list.
	collapsed->clear();
	delete collapsed;

	// Delete entire list, including items.
	delete expanded;
}


void
AWDGeomUtil::append_vert_data(unsigned int idx, double x, double y, double z,
    double u, double v, double nx, double ny, double nz, bool force_hard)
{
    vdata *vd;

    vd = (vdata *)malloc(sizeof(vdata));
    vd->orig_idx = idx;
    vd->out_idx = -1;
    vd->x = x;
    vd->y = y;
    vd->z = z;
    vd->u = u;
    vd->v = v;
    vd->nx = nx;
    vd->ny = ny;
    vd->nz = nz;
    vd->mtlid = 0;
    vd->num_bindings = 0;
    vd->weights = NULL;
    vd->joints = NULL;
    vd->force_hard = force_hard;

    append_vdata_struct(vd);
}


void
AWDGeomUtil::append_vdata_struct(vdata *vd)
{
	vd->first_normal_influence = NULL;
	vd->last_normal_influence = NULL;
	vd->out_idx = 0;
	expanded->append_vdata(vd);

	if (vd->orig_idx > max_orig_idx)
		max_orig_idx = vd->orig_idx;
}


static inline void
add_unique_influence(vdata *vd, double nx, double ny, double nz)
{
    if (!vd->first_normal_influence) {
        ninfluence *inf = (ninfluence *)malloc(sizeof(ninfluence));
        inf->nx = nx;
        inf->ny = ny;
        inf->nz = nz;
        inf->next = NULL;
        vd->first_normal_influence = inf;
        vd->last_normal_influence = inf;
    }
    else {
        ninfluence *cur;
        ninfluence *inf;

        cur = vd->first_normal_influence;
        while (cur) {
            if (cur->nx==nx && cur->ny==ny && cur->nz==nz)
                return;

            cur = cur->next;
        }

        // If reached, no duplicates exist
        inf = (ninfluence *)malloc(sizeof(ninfluence));
        inf->nx = nx;
        inf->ny = ny;
        inf->nz = nz;
        inf->next = NULL;
        vd->last_normal_influence->next = inf;
        vd->last_normal_influence = inf;
    }
}


int
AWDGeomUtil::has_vert(vdata *vd)
{
    int idx;
    vdata *cur;
	VertexDataList *list;

    // The list that is used for look-up is a list that only contains
	// vertex data structs that originate from the same client vertex.
	// Vertices that weren't the same in the client app should never
	// be joined together, and this optimization stems from that fact.
	list = per_idx_lists[vd->orig_idx];

    idx = 0;
	list->iter_reset();
	cur = list->iter_next();
	while (cur) {

        // If any of vertices have force_hard set, their normals must
        // not be averaged. Hence, they must be returned by this 
        // function as separate verts.
        if (cur->force_hard || vd->force_hard)
            goto next;

        // If position doesn't match, move on to next vertex
        if (cur->x != vd->x || cur->y != vd->y || cur->z != vd->z)
            goto next;

        // If UV coordinates do not match, move on to next vertex
        if (this->include_uv) {
            if (cur->u != vd->u || cur->v != vd->v)
				goto next;
        }

        // Check if normals match (possibly with fuzzy matching using a
        // threshold) and if they don't, move on to the next vertex.
        if (this->include_normals) {
            if (this->normal_threshold > 0) {
                if (vd->nx==cur->nx && vd->ny==cur->ny && vd->nz==cur->nz) {
                    // The exact same normals; avoid angle calculation
                    add_unique_influence(cur, vd->nx, vd->ny, vd->nz);
                }
                else {
                    double angle;
                    double l0, l1;

                    // Calculate lenghts (usually 1.0)
                    l0 = sqrt(cur->nx*cur->nx + cur->ny*cur->ny + cur->nz*cur->nz);
                    l1 = sqrt(vd->nx*vd->nx + vd->ny*vd->ny + vd->nz*vd->nz);

                    // Calculate angle and compare to threshold
                    angle = acos((cur->nx*vd->nx + cur->ny*vd->ny + cur->nz*vd->nz) / (l0*l1));
                    if (angle <= this->normal_threshold) {
                        add_unique_influence(cur, vd->nx, vd->ny, vd->nz);
                    }
                    else {
                        goto next;
                    }
                }
            }
            else if (cur->nx != vd->nx || cur->ny != vd->ny || cur->nz != vd->nz)
                goto next;
		}

        // Important: Don't put any more tests here after the normal test, which
        // needs to come last since it will add normal influences, which should
        // not be included unless the vertex matches.

        // Made it here? Then vertices match!
        return cur->out_idx;

next:
        idx++;
		cur = list->iter_next();
    }

    // This is the first time that this vertex is encountered,
    // so it's own influence needs to be added.
    add_unique_influence(vd, vd->nx, vd->ny, vd->nz);

    return -1;
}


void
AWDGeomUtil::prepare_build()
{
	int i;
	
	num_idx_lists = this->max_orig_idx + 1;

	per_idx_lists = (VertexDataList**)malloc(num_idx_lists * sizeof(VertexDataList *));
	for (i=0; i<num_idx_lists; i++) {
		per_idx_lists[i] = new VertexDataList();
	}
}


int 
AWDGeomUtil::build_geom(AWDTriGeom *md)
{
    vdata *vd;
    AWDSubGeom *sub;
    AWD_field_type tri_str_type;

    int v_idx, i_idx;
    AWD_str_ptr v_str;
    AWD_str_ptr i_str;
    AWD_str_ptr n_str;
    AWD_str_ptr u_str;
    AWD_str_ptr w_str;
    AWD_str_ptr j_str;

	int num_exp = expanded->get_num_items();

    sub = new AWDSubGeom();
    v_str.f64 = (awd_float64*) malloc(sizeof(awd_float64) * 3 * num_exp);
    i_str.ui32 = (awd_uint32*) malloc(sizeof(awd_uint32) * num_exp);

    if (this->include_normals) 
        n_str.f64 = (awd_float64*) malloc(sizeof(awd_float64) * 3 * num_exp);

    if (this->include_uv)
        u_str.f64 = (awd_float64*) malloc(sizeof(awd_float64) * 2 * num_exp);

    if (this->joints_per_vertex > 0) {
        int max_num_vals = num_exp * this->joints_per_vertex;
        w_str.f64 = (awd_float64*) malloc(sizeof(awd_float64) * max_num_vals);
        j_str.ui32 = (awd_uint32*) malloc(sizeof(awd_uint32) * max_num_vals);

        memset(w_str.v, 0, max_num_vals * sizeof(awd_float64));
        memset(j_str.v, 0, max_num_vals * sizeof(awd_uint32));
    }

	// Prebuild a lookup list of vertices by their original index, so that
	// has_vert() can check only those vertices that used to be the same
	// client vertex, instead of looping over them all.
	prepare_build();

    v_idx = i_idx = 0;

	expanded->iter_reset();
	vd = expanded->iter_next();
	while (vd) {
        int idx = this->has_vert(vd);
        if (idx >= 0) {
            i_str.ui32[i_idx++] = idx;
        }
        else {
            v_str.f64[v_idx*3+0] = vd->x;
            v_str.f64[v_idx*3+1] = vd->y;
            v_str.f64[v_idx*3+2] = vd->z;

            if (this->include_uv) {
                u_str.f64[v_idx*2+0] = vd->u;
                u_str.f64[v_idx*2+1] = vd->v;
            }

            if (this->include_normals) {
                n_str.f64[v_idx*3+0] = vd->nx;
                n_str.f64[v_idx*3+1] = vd->ny;
                n_str.f64[v_idx*3+2] = vd->nz;
            }

            // If there are bindings, transfer them from 
            // array in vdata struct to output streams.
            if (vd->num_bindings>0) {
                int w_idx;
                int jpv = vd->num_bindings;
                for (w_idx=0; w_idx<jpv; w_idx++) {
                    w_str.f64[v_idx*jpv + w_idx] = vd->weights[w_idx];
                    j_str.ui32[v_idx*jpv + w_idx] = vd->joints[w_idx];
                }
            }

			vd->out_idx = v_idx;
            i_str.ui32[i_idx++] = v_idx++;

			collapsed->append_vdata(vd);
			per_idx_lists[vd->orig_idx]->append_vdata(vd);
        }

		vd = expanded->iter_next();
    }

    // Smoothing (averaging of normals) required?
    if (this->normal_threshold > 0) {
		collapsed->iter_reset();
		vd = collapsed->iter_next();
		while (vd) {
            int num_influences;
            double nx, ny, nz;
            ninfluence *inf;

            nx = ny = nz = 0.0;
            num_influences = 0;
            inf = vd->first_normal_influence;
            while (inf) {
                nx += inf->nx;
                ny += inf->ny;
                nz += inf->nz;
                inf = inf->next;
                num_influences++;
            }

            n_str.f64[vd->out_idx*3+0] = nx / num_influences;
            n_str.f64[vd->out_idx*3+1] = ny / num_influences;
            n_str.f64[vd->out_idx*3+2] = nz / num_influences;

			vd = collapsed->iter_next();
        }
    }

    // Reallocate the vertex buffer using final length after vertices were
    // joined. There's no need to reallocate the index buffer since the 
    // triangle count will not have changed.
    v_str.v = realloc(v_str.v, sizeof(awd_float64) * 3 * v_idx);

    // Choose stream type for the triangle stream depending on whether
    // all vertex indices can be represented by an uint16 or not.
    tri_str_type = (v_idx > 0xffff)? AWD_FIELD_UINT32 : AWD_FIELD_UINT16;

    sub = new AWDSubGeom();
    sub->add_stream(VERTICES, AWD_FIELD_FLOAT32, v_str, v_idx*3);
    sub->add_stream(TRIANGLES, tri_str_type, i_str, i_idx);

    if (this->include_normals) {
        // Reallocate buffer using actual length and add to sub-geom
        n_str.v = realloc(n_str.v, sizeof(awd_float64) * 3 * v_idx);
        sub->add_stream(VERTEX_NORMALS, AWD_FIELD_FLOAT32, n_str, v_idx*3);
    }

    if (this->include_uv) {
        // Reallocate buffer using actual length and add to sub-geom
        u_str.v = realloc(u_str.v, sizeof(awd_float64) * 2 * v_idx);
        sub->add_stream(UVS, AWD_FIELD_FLOAT32, u_str, v_idx*2);
    }

    if (this->joints_per_vertex > 0) {
        // Reallocate buffers using actual length and add to sub-geom
        w_str.v = realloc(w_str.v, sizeof(awd_float64) * v_idx * this->joints_per_vertex);
        j_str.v = realloc(j_str.v, sizeof(awd_uint32) * v_idx * this->joints_per_vertex);
        sub->add_stream(VERTEX_WEIGHTS, AWD_FIELD_FLOAT32, w_str, v_idx*this->joints_per_vertex);
        sub->add_stream(JOINT_INDICES, AWD_FIELD_UINT16, j_str, v_idx*this->joints_per_vertex);
    }

    md->add_sub_mesh(sub);

    return 1;
}


