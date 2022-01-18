import {controller, state, Views} from "./state.js";
import {create_user_card} from "./shared.js";

export class UsersView {

    async init() {
        await controller.get_users();

        let view = state.loaded_views[Views.Users];
        let container = document.createElement("div");
        container.setAttribute("id", "view_users")
        container.innerHTML = view;

        let list = container.getElementsByClassName("list_body")[0]

        container.getElementsByClassName("users_create_button")[0].onclick = () => {
            let element = container.getElementsByClassName("reader")[0]
            element.innerHTML = `
                <div class="">
                    <div class="compose_input"><label>Username</label><input class="users_username_input" type="text" /></div>
                    <div class="compose_input"><label>Password</label><input class="users_password_input" type="password" /></div>
                    <div class="compose_input"><label>Role</label><input class="users_role_input" type="text"/></div>
                    <div><button class="users_submit_button">Create</button></div>
                </div>
                `
            element.getElementsByClassName("users_submit_button")[0].onclick = async () => {
                let username_i = element.getElementsByClassName("users_username_input")[0]
                let password_i = element.getElementsByClassName("users_password_input")[0]
                let role_i = element.getElementsByClassName("users_role_input")[0]
                let username = username_i.value
                let password = password_i.value
                let role = role_i.value
                let payload = {
                    username,
                    role
                }
                if (username.length > 0 && password.length > 0 && role.length > 0) {
                    payload.password = password
                    let ok = await controller.create_user(payload)
                    controller.notify(ok ? "User Created" : "Failed to Create User")
                    controller.set_view(Views.Users)
                }
            }
        }

        for (let user of state.users) {
            let card = create_user_card(user["username"], user["role"])
            card.onclick = () => {
                let element = container.getElementsByClassName("reader")[0]
                element.innerHTML = `
                <div class="">
                    <div class="compose_input"><label>Username</label><input class="users_username_input" type="text" value="${user.username}" /></div>
                    <div class="compose_input"><label>Password</label><input class="users_password_input" type="password" /></div>
                    <div class="compose_input"><label>Role</label><input class="users_role_input" type="text" value="${user.role}" /></div>
                    <div><button class="users_update_button">Update</button></div>
                </div>
                `
                element.getElementsByClassName("users_update_button")[0].onclick = async () => {
                    let username_i = element.getElementsByClassName("users_username_input")[0]
                    let password_i = element.getElementsByClassName("users_password_input")[0]
                    let role_i = element.getElementsByClassName("users_role_input")[0]
                    let username = username_i.value
                    let password = password_i.value
                    let role = role_i.value
                    let payload = {
                        username,
                        role
                    }
                    if (password.length > 0)
                        payload.password = password
                    let ok = await controller.update_user(user.id, payload)
                    controller.notify(ok ? "Update Succeeded" : "Updated Failed")
                }
            }
            card.getElementsByClassName("ui_delete")[0].onclick = async (event) => {
                let ok = await controller.delete_user(user.id)
                if (ok) {
                    card.remove()
                }
                controller.notify(ok ? "User Deleted" : "Failed to Delete User")
                event.stopPropagation()
            }
            list.appendChild(card)
        }

        controller.set(container);
    }

}