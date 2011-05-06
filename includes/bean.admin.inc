<?php

// $Id:$

/**
 * @file
 * Bean Admin Page
 */

/**
 * Main page callback on the bean type
 */
function bean_type_admin_page() {
  $field_ui = module_exists('field_ui');
  $rows = array();

  foreach (bean_get_types() as $type => $bean_type) {
    $row = array();
    $row[] = array('data' => $bean_type->getLabel());

    // Edit and delete buttons
    if ($type->isEditable()) {
      $row[] = array('data' => l(t('Edit'), 'admin/structure/block_type/manage/' . $bean_type->buildURL() . '/edit'));
      $row[] = array('data' => l(t('Edit'), 'admin/structure/block_type/manage/' . $bean_type->buildURL() . '/delete'));
    }

    if ($field_ui) {
      // Manage fields.
      $row[] = array('data' => l(t('manage fields'), 'admin/structure/block_type/manage/' . $bean_type->buildURL() . '/fields'));

      // Display fields.
      $row[] = array('data' => l(t('manage display'), 'admin/structure/block_type/manage/' . $bean_type->buildURL() . '/display'));
    }
    $rows[] = $row;
  }

  $header = array(t('Name'), array('data' => t('Operations'), 'colspan' => $field_ui ? '6' : '4'));

  $build['bean_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('There are no Bean Types Available'),
  );
  return $build;
}

/**
 * Generates the bean type editing form.
 */
function bean_type_form($form, &$form_state, $bean_type = NULL) {

  // If bean_type is null then load an empty one
  if (isnull($bean_type)) {
    $plugin_info = array(
      'label' => NULL,
      'type' => NULL,
      'handler' => array(
        'path' => drupal_get_path('module', 'bean') . '/plugins',
        'file' => 'base.inc',
        'class' => 'bean_ui',
        'parent' => 'bean',
      ),
    );

    $bean_type = new bean_ui($plugin_info);
  }

  $disabled = !$bean_type->isEditable();

  if ($disabled) {
    drupal_set_message(t('This Block Type can not be edited'));
  }

  $form['#bean_type'] = $bean_type;

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $bean_type->getLabel(),
    '#description' => t('The human-readable name of this block type.'),
    '#required' => TRUE,
    '#size' => 30,
    '#disabled' => $disabled,
  );

  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($bean_type->type) ? $bean_type->type : '',
    '#maxlength' => 32,
    '#machine_name' => array(
      'exists' => 'bean_get_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this block type. It must only contain lowercase letters, numbers, and underscores.'),
    '#disabled' => $disabled,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Block type'),
    '#weight' => 40,
    '#disabled' => $disabled,
  );

  // This is a new bean type
  if (isset($plugin_info)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete Block type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('bean_type_form_submit'),
      '#disabled' => $disabled,
    );
  }
  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function bean_type_form_submit(&$form, &$form_state) {
  $bean_type = $form_state['values']['bean_type'];
  $bean_type->type = $form_state['values']['type'];
  $bean_type->setLabel($form_state['values']['label']);
  $bean_type->save();
  $form_state['redirect'] = 'admin/structure/block_types';
}

/**
 * Form API submit callback for the delete button.
 */
function bean_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/block_types/manage/' . $form_state['bean_type']->type . '/delete';
}

/**
 * Menu callback; delete a single content type.
 */
function bean_type_delete_confirm($form, &$form_state, $type) {
  $form['type'] = array('#type' => 'value', '#value' => $type->type);
  $form['name'] = array('#type' => 'value', '#value' => $type->getLabel());

  $message = t('Are you sure you want to delete the block type %type?', array('%type' => $type->getLabel()));
  $caption = '';

  $num_beans = db_query("SELECT COUNT(*) FROM {bean_type} WHERE type = :type", array(':type' => $type->type))->fetchField();
  if ($num_beans) {
    $caption .= '<p>' . format_plural($num_beans, '%type is used by 1 block on your site. If you remove this block type, you will not be able to edit the %type blocks and it may not display correctly.', '%type is used by @count pieces of content on your site. If you remove %type, you will not be able to edit the %type content and it may not display correctly.', array('%type' => $type->getLabel())) . '</p>';
  }

  $caption .= '<p>' . t('This action cannot be undone.') . '</p>';

  return confirm_form($form, $message, 'admin/structure/block_types', $caption, t('Delete'));
}

function node_type_delete_confirm_submit($form, &$form_state) {
  $form_state['values']['type']->delete();
}